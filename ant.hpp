#ifndef ANT_HPP_INCLUDED
#define ANT_HPP_INCLUDED

#include "graph.hpp"
#include <vector>
#include <functional>
#include <random>
#include <algorithm>
#include <stack>
#include "chooser.hpp"

class Ant {
public:
	typedef std::vector<edge> way;
private:
	std::function<bool(const graph&, const int start, const int finish, way&)> strategy;
	way out_way;
public:
	Ant( std::function< bool( const graph&, const int start, const int finish, way&)> strategy) : strategy(strategy) {};
	bool start(const graph& gr, const int start, const int finish) {
		out_way.clear();
		return strategy(gr, start, finish, out_way);
	}
	void clear_way() { out_way.clear(); }
	const way& get_way() const { return this->out_way; }
};

bool dfs(int v, const graph& gr, const int finish, Ant::way& out_way, std::vector < bool >& visited);

bool go_dfs_way(const graph& gr, const int start, const int finish, Ant::way& out_way);

// greedy -- increasing it increases chances for lighter edges
// sweet_tooth -- increasing it increases chances for edges with more sweetness
bool ant_dfs(int v, const graph& gr, const int finish, Ant::way& out_way, std::vector < bool >& visited,
	const double greedy, const double sweet_tooth);

template < int GREEDY_NUM, int GREEDY_DENOM, int SWEET_TOOTH_NUM, int SWEET_TOOTH_DENOM >
bool go_ant(const graph& gr, const int start, const int finish, Ant::way& out_way) {
	std::vector < bool > visited(gr.size(), false);
	return ant_dfs(start, gr, finish, out_way, visited, static_cast<double>(GREEDY_NUM) / GREEDY_DENOM,
		static_cast<double>(SWEET_TOOTH_NUM) / SWEET_TOOTH_NUM);
}

bool sweet_dfs(int v, const graph& gr, const int finish, Ant::way& out_way, std::vector < bool >& visited);

bool sweet_tooth_ant(const graph& gr, const int start, const int finish, Ant::way& out_way);

enum class Comv_res : std::int8_t { FOUND, NOT_YET, TO_LONG_SEARCH };

Comv_res comv_dfs(const int v, const graph& gr, const double greedy, const double sweet_tooth,
	int &total_viewed, std::vector < bool > &visited, int cnt_visited, std::vector < edge > &out_way,
	const double randomness);

template < int GREEDY_NUM, int GREEDY_DENOM, int SWEET_TOOTH_NUM, int SWEET_TOOTH_DENOM,
	int RAND_NUM, int RAND_DENOM >
bool saller_ant(const graph& gr, const int start, const int finish /*nobody cares about that*/, Ant::way& out_way) {
	int total_visited = 0;
	std::vector < bool > visited(gr.size(), false);
	return comv_dfs(start, gr, static_cast<double>(GREEDY_NUM) / GREEDY_DENOM,
		static_cast<double>(SWEET_TOOTH_NUM) / SWEET_TOOTH_DENOM, total_visited, visited, 0, out_way,
		static_cast<double>(RAND_NUM) / RAND_DENOM) == Comv_res::FOUND;
}

#endif
