#ifndef ALGORITHMS_BASIC_H
#define ALGORITHMS_BASIC_H

#include <set>
#include <vector>
#include <limits>
#include <algorithm>

#include "config.h"

// Fundamental algorithms.
// =======================

template<typename T>
bool all_equal(const T& x, const T& y) {
	return (x == y);
}

template<typename T, typename... Tail>
bool all_equal(const T& x, const T& y, const Tail&... tail) {
	return (x == y) && all_equal(y, tail...);
}

// Topological structure building algorithms.
// ==========================================

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

template <class PredMap>
tree build_tree(const PredMap& pm) {
	tree result;
	for (size_t i = 0; i < pm.size(); ++i) { // TODO: Get rid of these incorrect size_t for interation
		node_id u = pm[i], v = i;
		if (u == v) {
			continue;
		}
		result.add(u, v);
	}
	return result;
}

// Tological optimization algorithms.
// ==================================

namespace detail {

    // Stop conditions for the Dijkstra's algorithm implementation.
    // ------------------------------------------------------------

	struct dst_stop {
		node_id dst;
		bool operator()(node_id u) { return u == dst; }
	};

	struct never_stop {
		bool operator()(node_id) { return false; }
	};

    /// Dijkstra's algorithm raw implementation.
    ///
    /// @tparam Graph A Topology,
    /// @tparam Metric A Metric,
    /// @tparam Stop A functor defining the stop condition for the algorithm.
    ///
    /// @param g The graph,
    /// @param m The metric,
    /// @param stop The stop condition functor,
    /// @param src The source for the relaxation,
    /// @param out_preds The out parameter returning the predecessors' map,
    /// @param out_dists The out parameter returning the distances' map.
    ///
	template <class Graph, class Metric, typename Stop>
	void dijkstra_relax(
			const Graph& g, const Metric& m,
			Stop stop, node_id src,
			std::vector<node_id>& out_preds,
			std::vector<typename Metric::weight_type>& out_dists) {

		node_id N = static_cast<node_id>(nodes_count(g));
		std::set<node_id> open;
		open.insert(src);

		out_preds.resize(N);
		out_dists.clear();
		out_dists.resize(N, weight_limits<typename Metric::weight_type>::inf());
		out_dists[src] = weight_limits<typename Metric::weight_type>::zero();

		while (!open.empty()) {

			auto it = std::min_element(
                begin(open),
                end(open),
				[&out_dists](node_id x, node_id y) {
                    return out_dists[x] < out_dists[y];
                });

			node_id u = *it;
			if (stop(u)) {
				break;
			}

            std::for_each(
                g.out_begin(u),
                g.out_end(u),
                [u, &out_dists, &out_preds, &m, &open](node_id v) mutable {
                    typename Metric::weight_type new_dist = out_dists[u] + m(link(u, v));
                    if (new_dist < out_dists[v]) {
                        out_dists[v] = new_dist;
                        out_preds[v] = u;
                        open.insert(v);
                    }
                });

			open.erase(u);
		}
	}

    /// The Bellman-Ford relaxation raw implementation.
    ///
    /// @tparam Graph A Topology,
    /// @tparam Metric A Metric,
    ///
    /// @param g The graph,
    /// @param m The metric,
    /// @param src The source of the relaxation,
    /// @param out_preds The out parameter returning the predecessors' map,
    /// @param out_dists The out parameter returning the distances' map.
	template <class Graph, class Metric>
	void bellman_ford_relax(
			const Graph& g, const Metric& m,
			node_id src,
			std::vector<node_id>& out_preds,
			std::vector<typename Metric::weight_type>& out_dists) {

		const node_id N = static_cast<node_id>(nodes_count(g));

		out_preds.resize(N);
		out_dists.clear();
		out_dists.resize(N, weight_limits<typename Metric::weight_type>::inf());
		out_dists[src] = weight_limits<typename Metric::weight_type>::zero();

		for (node_id i = 0; i < (N - 1); ++i) {
			for_each_edge(g, [&out_dists, &out_preds, &m](node_id u, node_id v) {
				typename Metric::weight_type new_dist = out_dists[u] + m(link(u, v));
				if (out_dists[v] > new_dist) {
					out_dists[v] = new_dist;
					out_preds[v] = u;
				}
			});
		}
	}

}

// The convenient API for the topological optimization algorithms.
// ===============================================================

template <class Graph, class Metric>
path dijkstra(const Graph& g, const Metric& m, node_id src, node_id dst) {
	std::vector<node_id> preds;
	std::vector<typename Metric::weight_type> dists;
	detail::dijkstra_relax(g, m, detail::dst_stop{ dst }, src, preds, dists);
	return build_path(src, dst, preds);
}

template <class Graph, class Metric>
tree prim(const Graph& g, const Metric& m, node_id src) {
	std::vector<node_id> preds;
	std::vector<typename Metric::weight_type> dists;
	detail::dijkstra_relax(g, m, detail::never_stop{}, src, preds, dists);
	return build_tree(preds);
}

template <class Graph, class Metric>
path bellman_ford(const Graph& g, const Metric& m, node_id src, node_id dst) {
	std::vector<node_id> preds;
	std::vector<typename Metric::weight_type> dists;
	detail::bellman_ford_relax(g, m, src, preds, dists);
	return build_path(src, dst, preds);
}

#endif
