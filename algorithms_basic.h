#ifndef ALGORITHMS_BASIC_H
#define ALGORITHMS_BASIC_H

#include <set>
#include <vector>
#include <limits>
#include <algorithm>

#include "topology.h"
#include "weight.h"
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

// Topological structure alanysis algorithms.
// ==========================================

template <class Metric, class Topology>
typename Metric::weight_type metric_add(const Topology& t, const Metric& m)
{
    using Weight = typename Metric::weight_type;
    Weight result = weight_limits<Weight>::zero;

    for_each_edge(t, [&result, &m](const edge& e) {
        result += m(e);
    });

    return result;
}

// Topological structure building algorithms.
// ==========================================

template <class PredMap>
path build_path(node src, node dst, const PredMap& pm) {
	path result;
	node u = dst;
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
		node u = pm[i], v = i;
		if (u == v) {
			continue;
		}
		result.set(edge { u, v });
	}
	return result;
}

// Tological optimization algorithms.
// ==================================

namespace detail {

    // Stop conditions for the Dijkstra's algorithm implementation.
    // ------------------------------------------------------------

	struct dst_stop {
		node dst;
		bool operator()(node u) { return u == dst; }
	};

	struct never_stop {
		bool operator()(node) { return false; }
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
			Stop stop, node src,
			std::vector<node>& out_preds,
			std::vector<typename Metric::weight_type>& out_dists) {

		node N = static_cast<node>(nodes_count(g));
		std::set<node> open;
		open.insert(src);

		out_preds.resize(N);
		out_dists.clear();
		out_dists.resize(N, weight_limits<typename Metric::weight_type>::inf());
		out_dists[src] = weight_limits<typename Metric::weight_type>::zero();

		while (!open.empty()) {

			auto it = std::min_element(
                begin(open),
                end(open),
				[&out_dists](node x, node y) {
                    return out_dists[x] < out_dists[y];
                });

			node u = *it;
			if (stop(u)) {
				break;
			}

            std::for_each(
                g.out_begin(u),
                g.out_end(u),
                [u, &out_dists, &out_preds, &m, &open](node v) mutable {
                    typename Metric::weight_type new_dist = out_dists[u] + m(edge(u, v));
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
			node src,
			std::vector<node>& out_preds,
			std::vector<typename Metric::weight_type>& out_dists) {

		const node N = static_cast<node>(nodes_count(g));

		out_preds.resize(N);
		out_dists.clear();
		out_dists.resize(N, weight_limits<typename Metric::weight_type>::inf());
		out_dists[src] = weight_limits<typename Metric::weight_type>::zero();

		for (node i = 0; i < (N - 1); ++i) {
			for_each_edge(g, [&out_dists, &out_preds, &m](node u, node v) {
				typename Metric::weight_type new_dist = out_dists[u] + m(edge(u, v));
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
path dijkstra(const Graph& g, const Metric& m, node src, node dst) {
	std::vector<node> preds;
	std::vector<typename Metric::weight_type> dists;
	detail::dijkstra_relax(g, m, detail::dst_stop{ dst }, src, preds, dists);
	return build_path(src, dst, preds);
}

template <class Graph, class Metric>
tree prim(const Graph& g, const Metric& m, node src) {
	std::vector<node> preds;
	std::vector<typename Metric::weight_type> dists;
	detail::dijkstra_relax(g, m, detail::never_stop{}, src, preds, dists);
	return build_tree(preds);
}

template <class Graph, class Metric>
path bellman_ford(const Graph& g, const Metric& m, node src, node dst) {
	std::vector<node> preds;
	std::vector<typename Metric::weight_type> dists;
	detail::bellman_ford_relax(g, m, src, preds, dists);
	return build_path(src, dst, preds);
}

#endif
