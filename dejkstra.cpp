#include <iostream>
#include <vector> 
#include <set>
#include "graph.hpp"

void print_way(const std::vector < size_t> &parent, const size_t v) {
	if (parent[v] == v) {
		std::cout << v + 1;
		return;
	}
	print_way(parent, parent[v]);
	std::cout << "->" << v + 1;
}

int main() {
	size_t n, m, start, finish;
	std::cin >> n >> m >> start >> finish;
	--start, --finish;
	std::vector < edge > edges(2 * m);
	for (size_t i = 0; i < m; ++i) {
		int a, b; double w;
		std::cin >> a >> b >> w;
		edges[2 * i] = edge(a - 1, b - 1, i, w);
		edges[2 * i + 1] = edge(b - 1, a - 1, i, w);
	}
	graph gr(n, edges.begin(), edges.end());
	std::vector < bool > reached(n, false), touched(n, false);
	std::vector < double > ways(n, 0.0);
	touched[start] = true;
	std::set < std::pair < double, size_t > > queue;
	std::vector < size_t > parent(n);
	parent[start] = start;
	queue.insert( std::make_pair(0.0, start) );
	while (!queue.empty()) {
		const size_t cur_v = queue.begin()->second;
		queue.erase(queue.begin());
		reached[cur_v] = true;
		if (cur_v == finish) {
			break;
		}
		for (auto e : gr.get_edges_from_vertex(cur_v)) {
			const size_t to = e.get_end(cur_v);
			if (!touched[to] || ways[to] > ways[cur_v] + e.weight) {
				if (touched[to]) {
					queue.erase( std::make_pair(ways[to], to) );
				}
				ways[to] = ways[cur_v] + e.weight;
				queue.insert( std::make_pair(ways[to], to) );
				parent[to] = cur_v;
			}
			touched[to] = true;
		}
	}
	if (!reached[finish]) {
		std::cout << "No way" << std::endl;
		return 0;
	}
	std::cout << "Weight: " << static_cast<int>(ways[finish] + 0.5) << std::endl;
	print_way(parent, finish);
	std::cout << std::endl;
	return 0;
}
