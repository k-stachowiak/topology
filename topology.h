#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <map>
#include <cmath>
#include <deque>
#include <cassert>
#include <iterator>
#include <algorithm>

#include "config.h"

template <typename T, typename U>
struct pair_second {
	T operator()(const std::pair<T, U>& pr) {
		return pr.second;
	}
};

// concept Path p
//
// Implemented entirely in terms of a double ended queue.

using path = std::deque<node_id>;

// concept Tree t
//
// operations:
// - (member) add edge (one direction): node -> node -> ()
// - (member) add edge (both directions): node -> node -> ()
// - (member) adjacent nodes: node -> [node]
// - (friend) for each edge: callback -> ()

struct tree {
	std::multimap<node_id, node_id> m_impl;

	// Semiregular: by default
	// Regular:
	friend bool operator==(const tree& x, const tree& y) {
		return x.m_impl == y.m_impl;
	}

	friend bool operator!=(const tree& x, const tree& y) {
		return !(x == y);
	}

	// Operations:
	void add(node_id from, node_id to) {
		m_impl.insert(std::make_pair(from, to));
	}

	void add2(node_id from, node_id to) {
		add(from, to);
		add(to, from);
	}

	std::deque<node_id> operator()(node_id x) const {
		auto range = m_impl.equal_range(x);
		std::deque<node_id> result;
		std::transform(
			range.first, range.second,
			std::back_inserter(result),
			pair_second<node_id, node_id>());
		return result;
	}

	template <class Func>
	friend void for_each_edge(const tree& t, Func f) {
		for (const auto& pr : t.m_impl) {
			f(pr.first, pr.second);
		}
	}
};

// concept Graph g
//
// operations:
// - (member) adjacent nodes: node -> [node]
// - (friend) nodes count: g -> size_t
// - (friend) for each edge: callback -> ()

struct adj_list_graph {

	// This is an adjacency list implementation of the Graph concept.

	std::deque<std::deque<node_id>> adjacency;

	// Semiregular: by default
	// Regular:
	friend bool operator==(const adj_list_graph& x, const adj_list_graph& y) {
		return x.adjacency == y.adjacency;
	}

	friend bool operator!=(const adj_list_graph& x, const adj_list_graph& y) {
		return !(x == y);
	}

	// Operations:
	const std::deque<node_id>& operator()(node_id u) const {
		return adjacency[u];
	}

	friend size_t nodes_count(const adj_list_graph& g) {
		return g.adjacency.size();
	}

	template <class Func>
	friend void for_each_edge(const adj_list_graph& g, Func f) {
		size_t N = nodes_count(g);
		for (size_t u = 0; u < N; ++u) {
			for (node_id v : g.adjacency.at(u)) {
				f(u, v);
			}
		}
	}
};

struct adj_matrix_graph {

	// This is an adjacency matric implementation of the Graph concept.

	std::deque<bool> matrix;
	size_t nodes;

	template <typename I>
	adj_matrix_graph(I first, I last) :
		matrix{ first, last }
	{
		double nodes_dbl = sqrt(matrix.size());
		assert((nodes_dbl - (double)(size_t)nodes_dbl) == 0.0);
		nodes = static_cast<size_t>(nodes_dbl);
	}

	// Semiregular:
	adj_matrix_graph() : nodes{ 0 } {}
	~adj_matrix_graph() = default;
	adj_matrix_graph(const adj_matrix_graph& x) = default;
	adj_matrix_graph(adj_matrix_graph&& x) :
		matrix{ std::move(x.matrix) },
		nodes{ std::move(x.nodes) }
	{}
	adj_matrix_graph& operator=(const adj_matrix_graph& x) = default;
	adj_matrix_graph& operator=(adj_matrix_graph&& x) {
		matrix = std::move(x.matrix);
		nodes = std::move(x.nodes);
		return *this;
	}

	// Regular:
	friend bool operator==(const adj_matrix_graph& x, const adj_matrix_graph& y) {
		return x.nodes == y.nodes && x.matrix == y.matrix;
	}
	friend bool operator!=(const adj_matrix_graph& x, const adj_matrix_graph& y) {
		return !(x == y);
	}

	// Operations:
	std::deque<node_id> operator()(node_id u) const {
		std::deque<node_id> result;
		auto first = begin(matrix) + u * nodes;
		auto last = first + nodes;
		for (auto current = first; current != last; ++current) {
			if (*current) {
				result.push_back(std::distance(first, current));
			}
		}
		return result;
	}

	friend size_t nodes_count(const adj_matrix_graph& g) {
		return g.nodes;
	}

	template <class Func>
	friend void for_each_edge(const adj_matrix_graph& g, Func f) {
		size_t N = nodes_count(g);
		for (size_t u = 0; u < N; ++u) {
			for (size_t v = 0; v < N; ++v) {
				if (g.matrix[N * u + v]) {
					f(u, v);
				}
			}
		}
	}
};

#endif