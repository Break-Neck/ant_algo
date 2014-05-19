#ifndef GRAPH_HPP_INCLUDED
#define GRAPH_HPP_INCLUDED

#include <vector>
#include <assert.h>
#include <algorithm>

struct edge {
	int from, to, index;
	double weight;
	edge() {}
	edge(int from, int to, int index, double weight = 1.0) : from(from), to(to), index(index), weight(weight) {}
	const int get_end(const int vertex) const { assert(vertex == from || vertex == to);
		return vertex == from ? to : from; }
};

class graph {
private:
	std::vector < std::vector < edge > > data;
	std::vector < double > sweetness;
public:
	const std::vector < edge >& get_edges_from_vertex(int vertex) const { return data[vertex]; }
	const double get_sweetness(int index) const { return sweetness[index]; }
	template < typename It_type >
	graph (const int size, It_type begin, It_type end) {
		data.resize(size);
		int cnt_edges = 0;
		for (; begin != end; ++begin) {
			data[begin->from].push_back(*begin);
			++cnt_edges;
		}
		sweetness.resize(cnt_edges);
	}
	const int size() const { return data.size(); }
	void update_way(const std::vector<edge>& way, const double delta) {
		std::for_each(way.begin(), way.end(),
			[this, delta](const edge& e) { sweetness[e.index] += delta; } );
	}
	void decrease_sweetness(const double delta_mul) {
		std::for_each(sweetness.begin(), sweetness.end(),
			[delta_mul](double& val) { val *= delta_mul; });
	}
};

#endif
