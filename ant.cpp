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
		probabilities[i] = std::pow(gr.get_edges_from_vertex(v)[i].weight, greedy) +
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
	int &total_viewed, std::vector < bool > &visited, int cnt_visited, std::vector < edge > &out_way,
	const double randomness) {

	const int BIT_RND = 16;
	if (static_cast<std::int64_t>(v) * v <= total_viewed) {
		return Comv_res::TO_LONG_SEARCH;
	}
	if (std::all_of(visited.begin(), visited.end(), [](const bool b) { return b; })) {
		return Comv_res::FOUND;
	}
	cnt_visited += !visited[v];
	visited[v] = true;
	++total_viewed;
	std::random_device rd;
	std::mt19937 gen(rd());
	int to = -1; 
	if (std::generate_canonical<double, BIT_RND>(gen) < randomness) { // while there is life there is hope
		std::uniform_int_distribution<> dis(0, gr.get_edges_from_vertex(v).size() - 1);
		to = dis(gen);
		out_way.push_back(gr.get_edges_from_vertex(v)[to]);
		Comv_res res;
		if ((res = comv_dfs(gr.get_edges_from_vertex(v)[to].get_end(v), gr, greedy, sweet_tooth, total_viewed,
			visited, cnt_visited, out_way, randomness)) == Comv_res::FOUND) {
			return Comv_res::FOUND;
		}
		out_way.pop_back();
		if (res == Comv_res::TO_LONG_SEARCH) {
			return Comv_res::TO_LONG_SEARCH;
		}
	}
	std::vector < double > probabilities(gr.get_edges_from_vertex(v).size());
	for (size_t i = 0; i < probabilities.size(); ++i) {
		probabilities[i] = std::pow(gr.get_edges_from_vertex(v)[i].weight, greedy) +
			std::pow(gr.get_sweetness(gr.get_edges_from_vertex(v)[i].index), sweet_tooth);
	}
	double sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0);
	std::for_each(probabilities.begin(), probabilities.end(), [&](double& val) { val /= sum; });
	chooser current_edges(probabilities.begin(), probabilities.end());
	while (!current_edges.empty()) {
		int next_edge_index = current_edges.get_next_index();
		if (next_edge_index == to) {
			continue;
		}
		const edge& next_edge = gr.get_edges_from_vertex(v)[next_edge_index];
		out_way.push_back(next_edge);
		Comv_res res;
		if ((res = comv_dfs(next_edge.get_end(v), gr, greedy, sweet_tooth, total_viewed,
			visited, cnt_visited, out_way, randomness)) == Comv_res::FOUND ) {
			return Comv_res::FOUND;
		}
		out_way.pop_back();
		if (res == Comv_res::TO_LONG_SEARCH) {
			return Comv_res::TO_LONG_SEARCH;
		}
	}
	return Comv_res::NOT_YET;
}

