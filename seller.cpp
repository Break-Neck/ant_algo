#include <iostream>
#include "graph.hpp"
#include "ant.hpp"
#include "chooser.hpp"
#include <exception>
#include <cmath>
#include <thread>
#include <future>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

inline void print_way(const std::vector < edge >& way) {
	std::cout << "way weight = " << std::accumulate(way.begin(), way.end(), 0.0,
		[&](const double old, const edge& e) { return old + e.weight; }) << std::endl;
	if (way.empty()) {
		return;
	}
	std::cout << way[0].from + 1;
	for (auto it : way) {
		std::cout << "->" << it.to + 1;
	}
	std::cout << std::endl;
}

void print_graph_edges(const graph& gr) {
	for (size_t v = 0; v < static_cast<size_t>(gr.size()); ++v) {
		for (const edge& e : gr.get_edges_from_vertex(v)) {
			if (e.from > e.to) {
				continue;
			}
			std::cout << e.from + 1 << "->" << e.to + 1 << " index: "
				<< e.index << " sweetness: " << gr.get_sweetness(e.index) 
				<< " weight: " << e.weight << std::endl;
		}
	}
}

const int DEF_GREEDY_NUM = -1;
const int DEF_GREEDY_DENOM = 1;
const int DEF_SWEET_TOOTH_NUM = 1;
const int DEF_SWEET_TOOTH_DENUM = 1;

inline void command_line_parsing(int argc, char** argv, size_t &ants, size_t& steps, double &sweetness_decreasing,
	double &add_param, bool &verbose, int &greedy_num, int &greedy_denom, int &sweet_tooth_num,
	int &sweet_tooth_denom) {
	
	const int DEF_WORKING_ANTS = 20;
	const int DEF_STEPS = 100;
	const double DEF_SWEETNESS_DECREASING = 0.1;
	const double DEF_ADD_PARAM = 1.0;

	po::options_description desc("Allowed options:");
	desc.add_options()
		("help,h", "produce help message")
		("verbose,v", "count steps")
		("ants", po::value<size_t>(&ants)->default_value(DEF_WORKING_ANTS), "number of ants")
		("steps", po::value<size_t>(&steps)->default_value(DEF_STEPS), "number of steps")
		("sweetdec", po::value<double>(&sweetness_decreasing)->default_value(DEF_SWEETNESS_DECREASING),
			"percent of sweetness decreasing")
		("addp", po::value<double>(&add_param)->default_value(DEF_ADD_PARAM), "adding coefficient")
		/*
		These parameters are set in compile time:

		("greednum", po::value<int>(&greedy_num)->default_value(DEF_GREEDY_NUM), "numerator of greed (must be negative!)")
		("greeddenom", po::value<int>(&greedy_denom)->default_value(DEF_GREEDY_DENOM), "denominator of greed")
		("sweetnum", po::value<int>(&sweet_tooth_num)->default_value(DEF_SWEET_TOOTH_NUM), "numirator of sweet toothing")
		("sweetdenom", po::value<int>(&sweet_tooth_denom)->default_value(DEF_SWEET_TOOTH_DENUM),
			"denominator of sweet toothing")

		*/
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		exit(EXIT_SUCCESS);
	}
	if (vm.count("verbose")) {
		verbose = true;
	}

}

void start_first_sellers(graph& gr, const size_t ants, const int start, const bool verbose,
	const double add_param, const double sweetness_decreasing) {
	std::vector < Ant > sellers(ants, Ant(random_seller));
	std::vector < bool > results(ants);

#ifdef ANT_PARALLEL
	std::vector < std::future<bool> > fut_results(ants);
	for (size_t i = 0; i < ants; ++i) {
		fut_results[i] = std::async(std::launch::async,
			&Ant::start, &(sellers[i]), std::cref(gr), start - 1, start - 1);
	}
	for (size_t i = 0; i < ants; ++i) {
		results[i] = fut_results[i].get();
	}
#else
	for (size_t i = 0; i < ants; ++i) {
		results[i] = sellers[i].start(gr, start - 1, start - 1);
	}
#endif
	for (size_t i = 0; i < ants; ++i) {
		if (results[i]) {
			double weight = 0.0;
			for (const edge& e: sellers[i].get_way()) {
				weight += e.weight;
			}
			gr.update_way(sellers[i].get_way(), add_param / weight);
		}
	}
	gr.decrease_sweetness(1.0 - sweetness_decreasing);
	if (verbose) {
		std::cout << "Random pathes has been made." << std::endl;
		std::cout << "Good start ways: " << std::count_if(results.begin(), results.end(),
			[](const bool b) { return b; }) << std::endl;
	}
}

inline double way_weight(const std::vector < edge >& way) {
	double res = 0.0;
	for (const edge& e : way) {
		res += e.weight;
	}
	return res;
}

int main(int argc, char** argv) {
	size_t ants, steps;
	double sweetness_decreasing, add_param;
	bool verbose = false;
	int greedy_num, greedy_denom, sweet_tooth_num, sweet_tooth_denom;
	command_line_parsing(argc, argv, ants, steps, sweetness_decreasing, add_param, verbose, greedy_num,
		greedy_denom, sweet_tooth_num, sweet_tooth_denom);
	int n, m, start, finish;
	std::cin >> n >> m >> start >> finish;
	std::vector < edge > edges(m);
	for (int i = 0; i < m; ++i) {
		int a, b; double w;
		std::cin >> a >> b >> w;
		edges[i] = edge(a - 1, b - 1, i, w);
	}
	graph gr(n, edges.begin(), edges.end());
	//First ants go random
	start_first_sellers(gr, ants, start, verbose, add_param, sweetness_decreasing);
	std::vector< Ant > sellers(ants, Ant(seller_ant<-1, 1, 1, 1, 1, 20>));
	std::vector < bool > results(ants);
	
	double best_ans = -1.0; //Starting value will be changed on the first iteration
	std::vector < edge > best_way;
	for (size_t step = 0; step < steps + 1; ++step) {
		int good_ways = 0;
#ifndef ANT_PARALLEL
		for (size_t i = 0; i < ants; ++i) {
			results[i] = sellers[i].start(gr, start - 1, start - 1);
		}
#else
		std::vector < std::future < bool > > fut_results(ants);
		for (size_t i = 0; i < ants; ++i) {
			fut_results[i] = std::async(std::launch::async,
				&Ant::start, &(sellers[i]), std::cref(gr), start - 1, start - 1);
		}
		for (size_t i = 0; i < ants; ++i) {
			results[i] = fut_results[i].get();
		}
#endif
		for (size_t i = 0; i < ants; ++i) {
			if (results[i]) {
				++good_ways;
				const double weight = way_weight(sellers[i].get_way());
				gr.update_way(sellers[i].get_way(), add_param / weight);
				if (weight < best_ans || best_ans < 0.0) {
					best_ans = weight;
					/* It should be "best_way = sellers[i].get_way()" but
					due to perfomance improvement it is like that, keeping in mind
		--------->		that the ways will not be used any more */
					best_way.swap(const_cast< std::vector< edge >& >(sellers[i].get_way()));
				}
			}
		}
		gr.decrease_sweetness(1.0 - sweetness_decreasing);
		if (verbose) {
			std::cout << "Step " << step + 1 << " has done, " << good_ways
				<< " good ways, best weight is " << best_ans << std::endl;
		}
	}
	print_way(best_way);
	return 0;
}
