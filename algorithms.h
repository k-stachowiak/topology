#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <set>
#include <vector>
#include <limits>
#include <algorithm>

#include "config.h"

// Fundamental algorithms.

template<typename T>
bool all_equal(const T& x, const T& y) {
	return (x == y);
}

template<typename T, typename... Tail>
bool all_equal(const T& x, const T& y, const Tail&... tail) {
	return (x == y) && all_equal(y, tail...);
}

// Path building algorithm.
template <class PredMap>
path build_path(node_id src, node_id dst, const PredMap& pm) {
	path result;
	node_id u = dst;
	while (u != src) {
		result.push_front(u);
		u = pm[u];
	}
	result.push_front(src);
	return result;
}

// Tree building algorithm.
template <class PredMap>
tree build_tree(const PredMap& pm) {
	tree result;
	for (size_t i = 0; i < pm.size(); ++i) {
		node_id u = pm[i], v = i;
		if (u == v) {
			continue;
		}
		result.add(u, v);
	}
	return result;
}

// Path and tree finding algoritms.

// Implementation details.
namespace detail {

	struct dst_stop {
		node_id dst;
		bool operator()(node_id u) {
			return u == dst;
		}
	};

	struct never_stop {
		bool operator()(node_id) {
			return false;
		}
	};

	template <class Graph, class MetricMap, typename Stop>
	void dijkstra_relax(
			const Graph& g, const MetricMap& mm,
			Stop stop, node_id src,
			std::vector<node_id>& out_preds,
			std::vector<typename MetricMap::value_type>& out_dists) {

		node_id N = static_cast<node_id>(nodes_count(g));
		std::set<node_id> open;
		open.insert(src);

		out_preds.resize(N);
		out_dists.clear();
		out_dists.resize(N, weight_limits<typename MetricMap::value_type>::inf());
		out_dists[src] = weight_limits<typename MetricMap::value_type>::zero();

		while (!open.empty()) {

			auto it = std::min_element(begin(open), end(open),
				[&out_dists](node_id x, node_id y){ return out_dists[x] < out_dists[y]; });

			node_id u = *it;
			if (stop(u)) {
				break;
			}

            std::for_each(
                    g.out_begin(u), g.out_end(u),
                    [u, &out_dists, &out_preds, &mm, &open](node_id v) mutable {
				typename MetricMap::value_type new_dist = out_dists[u] + mm(u, v);
				if (new_dist < out_dists[v]) {
					out_dists[v] = new_dist;
					out_preds[v] = u;
					open.insert(v);
				}
			});

			open.erase(u);
		}
	}

	template <class Graph, class MetricMap>
	void bellman_ford_relax(
			const Graph& g, const MetricMap& mm,
			node_id src,
			std::vector<node_id>& out_preds,
			std::vector<typename MetricMap::value_type>& out_dists) {

		const node_id N = static_cast<node_id>(nodes_count(g));

		out_preds.resize(N);
		out_dists.clear();
		out_dists.resize(N, weight_limits<typename MetricMap::value_type>::inf());
		out_dists[src] = weight_limits<typename MetricMap::value_type>::zero();

		for (node_id i = 0; i < (N - 1); ++i) {
			for_each_edge(g, [&out_dists, &out_preds, &mm](node_id u, node_id v) {
				typename MetricMap::value_type new_dist = out_dists[u] + mm(u, v);
				if (out_dists[v] > new_dist) {
					out_dists[v] = new_dist;
					out_preds[v] = u;
				}
			});
		}
	}

}

// Dijkstra's algorithm.
template <class Graph, class MetricMap>
path dijkstra(const Graph& g, const MetricMap& mm, node_id src, node_id dst) {
	std::vector<node_id> preds;
	std::vector<typename MetricMap::value_type> dists;
	detail::dijkstra_relax(g, mm, detail::dst_stop{ dst }, src, preds, dists);
	return build_path(src, dst, preds);
}

// Prim's algorithm.
template <class Graph, class MetricMap>
tree prim(const Graph& g, const MetricMap& mm, node_id src) {
	std::vector<node_id> preds;
	std::vector<typename MetricMap::value_type> dists;
	detail::dijkstra_relax(g, mm, detail::never_stop{}, src, preds, dists);
	return build_tree(preds);
}

// Bellman-Ford algorithm.
template <class Graph, class MetricMap>
path bellman_ford(const Graph& g, const MetricMap& mm, node_id src, node_id dst) {
	std::vector<node_id> preds;
	std::vector<typename MetricMap::value_type> dists;
	detail::bellman_ford_relax(g, mm, src, preds, dists);
	return build_path(src, dst, preds);
}

#endif
