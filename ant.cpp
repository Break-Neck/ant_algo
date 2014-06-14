#include "ant.hpp"

bool dfs(int v, const graph& gr, const int finish, Ant::way& out_way, std::vector < bool >& visited) {
	if (visited[v]) {
		return false;
	}
	visited[v] = true;
	if (v == finish) {
		return true;
	}
	for (auto it : gr.get_edges_from_vertex(v)) {
		if (!visited[it.get_end(v)]) {
			out_way.push_back(it);
			if (!dfs(it.get_end(v), gr, finish, out_way, visited)) {
				out_way.pop_back();
			} else {
				return true;
			}
		}
	}
	return false;
}

bool go_dfs_way(const graph& gr, const int start, const int finish, Ant::way& out_way) {
	std::vector < bool > visited(gr.size(), false);
	return dfs(start, gr, finish, out_way, visited);
}

bool ant_dfs(int v, const graph& gr, const int finish, Ant::way& out_way, std::vector < bool >& visited,
	const double greedy, const double sweet_tooth) {
	if (visited[v]) {
		return false;
	}
	visited[v] = true;
	if (v == finish) {
		return true;
	}
	std::vector < double > probabilities(gr.get_edges_from_vertex(v).size());
	for (size_t i = 0; i < probabilities.size(); ++i) {
		probabilities[i] = std::pow(gr.get_edges_from_vertex(v)[i].weight, greedy) *
			std::pow(gr.get_sweetness(gr.get_edges_from_vertex(v)[i].index), sweet_tooth);
	}
	double sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0);
	std::for_each(probabilities.begin(), probabilities.end(), [&](double& val) { val /= sum; });
	chooser current_edges(probabilities.begin(), probabilities.end());
	while (!current_edges.empty()) {
		const edge& next_edge = gr.get_edges_from_vertex(v)[current_edges.get_next_index()];
		if (!visited[next_edge.get_end(v)]) {
			out_way.push_back(next_edge);
			if (!ant_dfs(next_edge.get_end(v), gr, finish, out_way, visited, greedy, sweet_tooth)) {
				out_way.pop_back();
			} else {
				return true;
			}
		}
	}
	return false;
}

bool sweet_dfs(int v, const graph& gr, const int finish, Ant::way& out_way, std::vector < bool >& visited) {
	if (visited[v]) {
		return false;
	}
	visited[v] = true;
	if (v == finish) {
		return true;
	}
	std::vector < edge > cur_edges(gr.get_edges_from_vertex(v));
	//the sweetest edges are on the top
	std::sort(cur_edges.begin(), cur_edges.end(), [&](const edge& e1, const edge& e2)
		{ return gr.get_sweetness(e1.index) > gr.get_sweetness(e2.index); } );
	for (const edge& e : cur_edges) {
		if (!visited[e.get_end(v)]) {
			out_way.push_back(e);
			if (sweet_dfs(e.get_end(v), gr, finish, out_way, visited)) {
				return true;
			}
			out_way.pop_back();
		}
	}
	return false;
}


bool sweet_tooth_ant(const graph& gr, const int start, const int finish, Ant::way& out_way) {
	std::vector < bool > visited(gr.size(), false);
	return sweet_dfs(start, gr, finish, out_way, visited);
}

Comv_res comv_dfs(const int v, const graph& gr, const double greedy, const double sweet_tooth,
	size_t &total_viewed, const size_t operation_limit, std::vector < int > &visited,
	size_t &cnt_visited, int deep, std::vector < edge > &out_way, const double randomness) {

	const int BIT_RND = 16;
	if (++deep * 10LL >= gr.size() * 21LL) {
		return Comv_res::NOT_YET;
	}
	if (++total_viewed >= operation_limit) {
		return Comv_res::TOO_LONG_SEARCH;
	}
	cnt_visited += !visited[v];
	++visited[v];
	if (cnt_visited >= static_cast<size_t>(gr.size())) {
		return Comv_res::FOUND;
	}
	assert( std::count(visited.begin(), visited.end(), true) < gr.size() );
	std::random_device rd;
	std::mt19937 gen(rd());
	if (std::generate_canonical<double, BIT_RND>(gen) < randomness) {
		const size_t unvisited = std::count(visited.begin(), visited.end(), 0);
		std::uniform_int_distribution<> dis(1, unvisited);
		size_t next_vert_ind = dis(gen);
		size_t next_vert = 0;
		for (size_t i = 0; next_vert_ind; ++i) {
			if (!visited[i]) {
				--next_vert_ind;
			}
			next_vert = i;
		}
		std::vector < edge > cur_way;
		find_any_way(gr, v, next_vert, cur_way);
		const size_t new_edges = cur_way.size();
		if ((new_edges - 1 + deep) * 10LL < gr.size() * 21LL) {
			size_t cur_v = v;
			for (const edge& e : cur_way) {
				if (cur_v != static_cast<size_t>(v)) {
					if (!visited[cur_v]) {
						++cnt_visited;
					}
					++visited[cur_v];
				}
				cur_v = e.get_end(cur_v);
				out_way.push_back(e);
			}
			cur_way.clear();
			cur_way.shrink_to_fit();
			Comv_res res;
			total_viewed += new_edges - 1;
			if ((res = comv_dfs(next_vert, gr, greedy, sweet_tooth, total_viewed, operation_limit, visited,
				cnt_visited, deep + new_edges - 1, out_way, randomness)) == Comv_res::FOUND) {
				return Comv_res::FOUND;
			}
			if (res == Comv_res::TOO_LONG_SEARCH) {
				return Comv_res::TOO_LONG_SEARCH;
			}
			cur_v = next_vert;
			for (size_t i = 0; i < new_edges; ++i) {
				if (cur_v != static_cast<size_t>(next_vert)) {
					if (!(--visited[cur_v])) {
						--cnt_visited;
					}
				}
				cur_v = out_way.back().get_end(cur_v);
				out_way.pop_back();
			}
		}
	}
	std::vector < double > probabilities(gr.get_edges_from_vertex(v).size());
	for (size_t i = 0; i < probabilities.size(); ++i) {
		probabilities[i] = std::pow(gr.get_edges_from_vertex(v)[i].weight, greedy) *
			std::pow(gr.get_sweetness(gr.get_edges_from_vertex(v)[i].index), sweet_tooth);
	}
	double sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0);
	std::for_each(probabilities.begin(), probabilities.end(), [&](double& val) { val /= sum; });
	chooser current_edges(probabilities.begin(), probabilities.end());
	while (!current_edges.empty()) {
		int next_edge_index = current_edges.get_next_index();
		const edge& next_edge = gr.get_edges_from_vertex(v)[next_edge_index];
		out_way.push_back(next_edge);
		Comv_res res;
		if ((res = comv_dfs(next_edge.get_end(v), gr, greedy, sweet_tooth, total_viewed,
			operation_limit, visited, cnt_visited, deep, out_way, randomness)) == Comv_res::FOUND ) {
			return Comv_res::FOUND;
		}
		if (res == Comv_res::TOO_LONG_SEARCH) {
			return Comv_res::TOO_LONG_SEARCH;
		}
		out_way.pop_back();
	}
	if (!(--visited[v])) {
		--cnt_visited;
	}
	assert(visited[v] >= 0);
	return Comv_res::NOT_YET;
}

bool find_any_way(const graph& gr, const int start, const int end, std::vector < edge > &out_way) {
	std::queue < int > q;
	std::vector < bool > visited(gr.size(), false);
	std::vector < edge > edge_from(gr.size(), edge(-1, -1, -1, -1));
	q.push(start);
	visited[start] = true;
	while (!q.empty()) {
		int cur_v = q.front();
		q.pop();
		if (cur_v == end) {
			break;
		}
		for (const edge& e : gr.get_edges_from_vertex(cur_v)) {
			if (visited[e.get_end(cur_v)]) {
				continue;
			}
			visited[e.get_end(cur_v)] = true;
			q.push(e.get_end(cur_v));
			edge_from[e.get_end(cur_v)] = e;
		}
	}
	out_way.clear();
	if (!visited[end]) {
		return false;
	}
	int cur_v = end;
	while (cur_v != start) {
		out_way.push_back(edge_from[cur_v]);
		cur_v = edge_from[cur_v].get_end(cur_v);
	}
	std::reverse(out_way.begin(), out_way.end());
	return true;
}

bool random_seller(const graph& gr, const int start, const int finish /*nobody cares */, Ant::way& out_way) {
	std::vector < int > verts(gr.size());
	for (size_t i = 0; i < verts.size(); ++i) {
		verts[i] = i;
	}
	std::random_shuffle(verts.begin(), verts.end()); //May be improved with std::shuffle
	std::vector < bool > visited(gr.size(), false);
	visited[start] = true;
	int number_of_visited = 1;
	int cur_v = start;
	std::vector < edge > cur_way;
	out_way.clear();
	for (size_t i = 0; i < verts.size(); ++i) {
		const int to = verts[i];
		if (to == start) {
			continue;
		}
		cur_way.clear();
		if (!find_any_way(gr, cur_v, to, cur_way)) {
			return false;
		}
		for (const edge& e : cur_way) {
			if (!visited[e.get_end(cur_v)]) {
				++number_of_visited;
				visited[e.get_end(cur_v)] = true;
			}
			cur_v = e.get_end(cur_v);
			out_way.push_back(e);
			if (number_of_visited == gr.size()) {
				break;
			}
		}
		if (number_of_visited == gr.size()) {
			return true;
		}
	}
	assert(number_of_visited == gr.size());
	return false;
}

