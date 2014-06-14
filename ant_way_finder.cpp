#include <iostream>
#include "graph.hpp"
#include "ant.hpp"
#include "chooser.hpp"
#include <exception>
#include <cmath>
#include <thread>
#include <future>
#include <algorithm>
#include <ctime>

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

int main(int argc, char** argv) {
	size_t ants, steps;
	double sweetness_decreasing, add_param;
	bool verbose = false;
	int greedy_num, greedy_denom, sweet_tooth_num, sweet_tooth_denom;
	command_line_parsing(argc, argv, ants, steps, sweetness_decreasing, add_param, verbose, greedy_num,
		greedy_denom, sweet_tooth_num, sweet_tooth_denom);
	int n, m, start, finish;
	srand(time(NULL));
	std::cin >> n >> m >> start >> finish;
	std::vector < edge > edges(m);
	for (int i = 0; i < m; ++i) {
		int a, b; double w;
		std::cin >> a >> b >> w;
		edges[i] = edge(a - 1, b - 1, i, w);
	}
	graph gr(n, edges.begin(), edges.end());
	std::vector< Ant > workers(ants, Ant(go_ant<-1, 1, 1, 1>));
	std::vector < bool > results(ants);
	for (size_t step = 0; step < steps; ++step) {
#ifndef ANT_PARALLEL
		for (size_t i = 0; i < ants; ++i) {
			results[i] = workers[i].start(gr, start - 1, finish - 1);
		}
#else
		std::vector < std::future < bool > > fut_results(ants);
		for (size_t i = 0; i < ants; ++i) {
			fut_results[i] = std::async(std::launch::async,
				&Ant::start, &(workers[i]), std::cref(gr), start - 1, finish - 1);
		}
		for (size_t i = 0; i < ants; ++i) {
			results[i] = fut_results[i].get();
		}
#endif
		for (size_t i = 0; i < ants; ++i) {
			if (results[i]) {
				gr.update_way(workers[i].get_way(), add_param / workers[i].get_way().size());
			}
		}
		gr.decrease_sweetness(1.0 - sweetness_decreasing);
		if (verbose) {
			std::cout << "Step " << step + 1 << " has done" << std::endl;
		}
	}
	Ant final_ant(sweet_tooth_ant);
	if (final_ant.start(gr, start - 1, finish - 1)) {
		std::cout << "Found way: ";
		print_way(final_ant.get_way());
	}
	return 0;
}
