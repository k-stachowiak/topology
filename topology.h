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
// - (member) out begin/end: node -> iterator(node)
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

    std::multimap<node_id, node_id>::const_iterator out_begin(node_id x) const {
        return m_impl.equal_range(x).first;
    }

    std::multimap<node_id, node_id>::const_iterator out_end(node_id x) const {
        return m_impl.equal_range(x).second;
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
// - (member) out begin/end: node -> iterator(node)
// - (member) set: node -> node -> ()
// - (member) set2: node -> node -> ()  -- bidirectional
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
    std::deque<node_id>::const_iterator out_begin(node_id x) const {
        return adjacency[x].begin();
    }

    std::deque<node_id>::const_iterator out_end(node_id x) const {
        return adjacency[x].end();
    }

    void set(node_id from, node_id to) {
        if (from >= static_cast<node_id>(adjacency.size())) {
            adjacency.resize(from + 1);
        }
        adjacency[from].push_back(to);
    }

    void set2(node_id from, node_id to) {
        set(from, to);
        set(to, from);
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

    struct out_iterator;

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
    out_iterator out_begin(node_id u) const;
    out_iterator out_end(node_id u) const;

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

    void set(node_id from, node_id to) {

        if (static_cast<node_id>(matrix.size()) > (from * from)) {
            matrix[nodes * from + to] = true;
            return;
        }

        size_t new_nodes = from + 1;
        std::deque<bool> new_matrix(new_nodes * new_nodes, false);

        for (size_t f = 0; f < nodes; ++f) {
            for (size_t t = 0; t < nodes; ++t) {
                new_matrix[new_nodes * f + t] = matrix[nodes * f + t];
            }
        }

        new_matrix[new_nodes * from + to] = true;

        nodes = new_nodes;
        matrix = std::move(new_matrix);
    }

    void set2(node_id from, node_id to) {
        set(from, to);
        set(to, from);
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

// Implementation of an outgoing iterator for an adjacency matrix.
// It will traverse an adjacency matrix row, skipping false entries.
struct adj_matrix_graph::out_iterator :
        std::iterator<std::forward_iterator_tag, node_id> {

    typedef std::deque<bool>::const_iterator impl_type;

    impl_type first, last;
    impl_type current;

    // Semiregular:
    out_iterator() = default;
    out_iterator(const out_iterator&) = default;
    out_iterator& operator=(const out_iterator&) = default;

    friend bool operator==(const out_iterator& x, const out_iterator& y) {
        return x.current == y.current;
    }

    friend bool operator!=(const out_iterator& x, const out_iterator& y) {
        return !(x == y);
    }

    // Custom constructor.
    out_iterator(impl_type first, impl_type last) :
        first { first }, last { last }, current { first }
    {}

    // Iterator specific operations.
    out_iterator& operator++() {
        current = std::find(current + 1, last, true);
        return *this;
    }

    const out_iterator operator++(int) {
        out_iterator copy = *this;
        ++(*this);
        return copy;
    }

    node_id operator*() const {
        return std::distance(first, current);
    }
};

adj_matrix_graph::out_iterator adj_matrix_graph::out_begin(node_id u) const {

    auto first = begin(matrix) + u * nodes;
    auto last = first + nodes;
    out_iterator result { first, last };

    auto first_true = std::find(first, last, true);
    result.current = first_true;

    return result;
}

adj_matrix_graph::out_iterator adj_matrix_graph::out_end(node_id u) const {
    auto last = begin(matrix) + u * nodes + nodes;
    return out_iterator { last, last };
}

#endif
