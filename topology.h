#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <map>
#include <cmath>
#include <deque>
#include <cassert>
#include <iterator>
#include <algorithm>

#include "config.h"

#if 0
concept Topology : Regular {
    static NaturalZero nodes_count(Topology);
    static ForwardIterator<node> out_begin(Topology, node);
    static ForwardIterator<node> out_end(Topology, node);
    static ForwardIterator<edge> edge_begin(Topology);
    static ForwardIterator<edge> edge_end(Topology);
}

concept Graph : Topology {
    void set(edge);
}

concept Path : Topology {
    void push_front(node);
    void push_back(node);
}

concept Tree : Topology {
    void set(edge);
}
#endif

// Graph concept implementations.
// ==============================

struct adj_list_graph {

    std::deque<std::deque<node>> adjacency;

    struct const_edge_iterator : std::iterator<std::forward_iterator_tag, edge> {

        const adj_list_graph *graph;
        int nd, adj;

        const_edge_iterator() : graph { nullptr }, nd { -1 }, adj { -1 } {}
        const_edge_iterator(const const_edge_iterator&) = default;
        const_edge_iterator(const_edge_iterator&&) = default;
        const_edge_iterator& operator=(const const_edge_iterator&) = default;
        const_edge_iterator& operator=(const_edge_iterator&&) = default;

        const_edge_iterator(const adj_list_graph *g, int n, int a) : graph { g }, nd { n }, adj { a } {}

        friend bool operator==(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return x.graph == y.graph && x.nd == y.nd && x.adj == y.adj;
        }

        friend bool operator!=(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return !(x == y);
        }

        const_edge_iterator& operator++()
        {
            if (++adj == static_cast<int>(graph->adjacency.at(nd).size())) {
                ++nd;
                adj = 0;
            }
            return *this;
        }

        const const_edge_iterator operator++(int)
        {
            const_edge_iterator copy = *this;
            ++(*this);
            return copy;
        }

        edge operator*() const
        {
            return {
                static_cast<node>(nd),
                graph->adjacency.at(nd).at(adj),
            };
        }
    };

    // Semiregular: by default
    // Regular:
    friend bool operator==(const adj_list_graph& x, const adj_list_graph& y)
    {
        return x.adjacency == y.adjacency;
    }

    friend bool operator!=(const adj_list_graph& x, const adj_list_graph& y)
    {
        return !(x == y);
    }

    // Operations:
    void set(const edge& e)
    {
        node from = e.first;
        node to = e.second;
        if (from >= static_cast<node>(adjacency.size())) {
            adjacency.resize(from + 1);
        }
        adjacency[from].push_back(to);
    }

    friend int nodes_count(const adj_list_graph& g)
    {
        std::vector<node> nodes;

        for (decltype(g.adjacency)::size_type i = 0; i < g.adjacency.size(); ++i) {
            nodes.emplace_back(i);
            for (const auto& n : g.adjacency[i]) {
                nodes.push_back(n);
            }
        }

        std::sort(begin(nodes), end(nodes));

        return std::distance(begin(nodes), std::unique(begin(nodes), end(nodes)));
    }

    friend std::deque<node>::const_iterator out_begin(const adj_list_graph& g, node x)
    {
        return g.adjacency[x].begin();
    }

    friend std::deque<node>::const_iterator out_end(const adj_list_graph& g, node x)
    {
        return g.adjacency[x].end();
    }

    friend const_edge_iterator edge_begin(const adj_list_graph& g)
    {
        return { &g, 0, 0 };
    }

    friend const_edge_iterator edge_end(const adj_list_graph& g)
    {
        return { &g, static_cast<node>(g.adjacency.size()), static_cast<node>(0) };
    }
};

struct adj_matrix_graph {

    std::deque<bool> matrix;
    int nodes;

    struct out_iterator : std::iterator<std::forward_iterator_tag, node> {

        typedef std::deque<bool>::const_iterator impl_type;

        impl_type first, last;
        impl_type current;

        // Semiregular:
		~out_iterator() = default;
        out_iterator() = default;
        out_iterator(const out_iterator&) = default;
        out_iterator(out_iterator&&) = default;
        out_iterator& operator=(const out_iterator&) = default;
        out_iterator& operator=(out_iterator&&) = default;

        friend bool operator==(const out_iterator& x, const out_iterator& y)
        {
            return x.current == y.current;
        }

        friend bool operator!=(const out_iterator& x, const out_iterator& y)
        {
            return !(x == y);
        }

        // Custom constructor.
        out_iterator(impl_type first, impl_type last) :
            first { first }, last { last }, current { first }
        {}

        // Iterator specific operations.
        out_iterator& operator++()
        {
            current = std::find(current + 1, last, true);
            return *this;
        }

        const out_iterator operator++(int)
        {
            out_iterator copy = *this;
            ++(*this);
            return copy;
        }

        node operator*() const
        {
            return std::distance(first, current);
        }
    };

    struct const_edge_iterator : std::iterator<std::forward_iterator_tag, edge> {

        const adj_matrix_graph *graph;
        int u, v;

        const_edge_iterator() : graph { nullptr }, u { -1 }, v { -1 } {}
        const_edge_iterator(const const_edge_iterator&) = default;
        const_edge_iterator(const_edge_iterator&&) = default;
        const_edge_iterator& operator=(const const_edge_iterator&) = default;
        const_edge_iterator& operator=(const_edge_iterator&&) = default;

        const_edge_iterator(const adj_matrix_graph *g, int u, int v) : graph { g }, u { u }, v { v } {}

        friend bool operator==(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return x.graph == y.graph && x.u == y.u && x.v == y.v;
        }

        friend bool operator!=(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return !(x == y);
        }

        const_edge_iterator& operator++()
        {
            int N = nodes_count(*graph);

            do {
                if (++v == N) {
                    if (++u == N) {
                        break;
                    }
                    v = 0;
                }
            } while (!graph->matrix.at(N * u + v));

            return *this;
        }

        const const_edge_iterator operator++(int)
        {
            const_edge_iterator copy = *this;
            ++(*this);
            return copy;
        }

        edge operator*() const
        {
            return { static_cast<node>(u), static_cast<node>(v) };
        }
    };

    // Semiregular:
    adj_matrix_graph() : nodes{ 0 } {}
    ~adj_matrix_graph() = default;
    adj_matrix_graph(const adj_matrix_graph& x) = default;
    adj_matrix_graph(adj_matrix_graph&& x) = default;
    adj_matrix_graph& operator=(const adj_matrix_graph& x) = default;
    adj_matrix_graph& operator=(adj_matrix_graph&& x) = default;

    template <typename I>
    adj_matrix_graph(I first, I last) : matrix{ first, last }
    {
        double nodes_dbl = sqrt(matrix.size());
        assert((nodes_dbl - (double)(int)nodes_dbl) == 0.0);
        nodes = static_cast<int>(nodes_dbl);
    }

    // Regular:
    friend bool operator==(const adj_matrix_graph& x, const adj_matrix_graph& y)
    {
        return x.nodes == y.nodes && x.matrix == y.matrix;
    }

    friend bool operator!=(const adj_matrix_graph& x, const adj_matrix_graph& y)
    {
        return !(x == y);
    }

    // Operations:
    void set(const edge& e)
    {
        node from = e.first;
        node to = e.second;
        node max_index = std::max(from, to);

        if (static_cast<node>(matrix.size()) > (max_index * max_index)) {
            matrix[nodes * from + to] = true;
            return;
        }

        int new_nodes = max_index + 1;
        std::deque<bool> new_matrix(new_nodes * new_nodes, false);

        for (int f = 0; f < nodes; ++f) {
            for (int t = 0; t < nodes; ++t) {
                new_matrix[new_nodes * f + t] = matrix[nodes * f + t];
            }
        }

        new_matrix[new_nodes * from + to] = true;

        nodes = new_nodes;
        matrix = std::move(new_matrix);
    }

    friend int nodes_count(const adj_matrix_graph& g)
    {
        return g.nodes;
    }

    friend out_iterator out_begin(const adj_matrix_graph& g, node u)
    {
        auto first = begin(g.matrix) + u * g.nodes;
        auto last = first + g.nodes;
        out_iterator result { first, last };

        auto first_true = std::find(first, last, true);
        result.current = first_true;

        return result;
    }

    friend out_iterator out_end(const adj_matrix_graph& g, node u)
    {
        auto last = begin(g.matrix) + u * g.nodes + g.nodes;
        return out_iterator { last, last };
    }

    friend const_edge_iterator edge_begin(const adj_matrix_graph& g)
    {
        const_edge_iterator result { &g, 0, 0 };

        if (!g.matrix.at(0)) {
            ++result;
        }

        return result;
    }

    friend const_edge_iterator edge_end(const adj_matrix_graph& g)
    {
        int N = nodes_count(g);
        return { &g, static_cast<node>(N), static_cast<node>(N) };
    }

};

// Path concept implementations.
// =============================

using path = std::deque<node>;

struct path_out_iterator : std::iterator<std::forward_iterator_tag, node> {
	enum Phase {
		FIRST, SECOND, END
	} phase;
	node first, second;

	// Semiregular:
	~path_out_iterator() = default;
	path_out_iterator() = default;
	path_out_iterator(const path_out_iterator&) = default;
	path_out_iterator(path_out_iterator&&) = default;
	path_out_iterator& operator=(const path_out_iterator&) = default;
	path_out_iterator& operator=(path_out_iterator&&) = default;

	// Regular:
	friend bool operator==(const path_out_iterator& x, const path_out_iterator& y)
	{
		if (x.phase != y.phase) {
			return false;
		}

		switch (x.phase) {
		case FIRST:
			return x.first == y.first;
		case SECOND:
			return x.second == y.second;
		case END:
			return true;
		}

		throw std::runtime_error { "Should never get here. " };
	}

	friend bool operator!=(const path_out_iterator& x, const path_out_iterator& y)
	{
		return !(x == y);
	}

	// Custom constructor:
	path_out_iterator(Phase phase, node first, node second) :
		phase { phase }, first { first }, second { second }
	{}

	// Iterator specific operations:
	path_out_iterator& operator++()
	{
		switch (phase) {
		case FIRST:
			phase = SECOND;
			break;
		case SECOND:
			phase = END;
			break;
		case END:
			break;
		}
		return *this;
	}

	const path_out_iterator operator++(int)
	{
		path_out_iterator copy = *this;
		++(*this);
		return copy;
	}

	node operator*() const
	{
		switch (phase) {
		case FIRST:
			return first;
		case SECOND:
			return second;
		default:
			return -1;
		}
	}
};

struct const_path_edge_iterator : std::iterator<std::forward_iterator_tag, edge> {

    const path *p;
    int i;

    const_path_edge_iterator() : p { nullptr }, i { -1 } {}
    const_path_edge_iterator(const const_path_edge_iterator&) = default;
    const_path_edge_iterator(const_path_edge_iterator&&) = default;
    const_path_edge_iterator& operator=(const const_path_edge_iterator&) = default;
    const_path_edge_iterator& operator=(const_path_edge_iterator&&) = default;

    const_path_edge_iterator(const path *p, int i) : p { p }, i { i } {}

    friend bool operator==(const const_path_edge_iterator& x, const const_path_edge_iterator& y)
    {
        return x.p == y.p && x.i == y.i;
    }

    friend bool operator!=(const const_path_edge_iterator& x, const const_path_edge_iterator& y)
    {
        return !(x == y);
    }

    const_path_edge_iterator& operator++()
    {
        ++i;
        return *this;
    }

    const const_path_edge_iterator operator++(int)
    {
        const_path_edge_iterator copy = *this;
        ++(*this);
        return copy;
    }

    edge operator*() const
    {
        return { p->at(i), p->at(i + 1) };
    }
};

inline int nodes_count(const path &p)
{
    return p.size();
}

inline path_out_iterator out_begin(const path& p, node n)
{
	if (n == p.front()) {
		return { path_out_iterator::SECOND, n, p.at(1) };
	} else if (n == p.back()) {
		return { path_out_iterator::SECOND, n, p.at(p.size() - 2) };
	} else {
		auto it = std::find(begin(p), end(p), n);
		return { path_out_iterator::FIRST, *(it - 1), *(it + 1) };
	}
}

inline path_out_iterator out_end(const path& p, node n)
{
	return { path_out_iterator::END, n, n };
}

inline const_path_edge_iterator edge_begin(const path& p)
{
    return { &p, 0 };
}

inline const_path_edge_iterator edge_end(const path& p)
{
    return { &p, static_cast<node>(p.size() - 1) };
}

// Tree concept implementations.
// =============================

struct tree {

    std::multimap<node, node> m_impl;

    struct out_iterator : std::iterator<std::forward_iterator_tag, node> {

        typedef std::multimap<node, node>::const_iterator impl_type;

        bool has_children;
        bool at_parent;
        node parent;
        node current;
        impl_type current_child;

        // Semiregular:
        ~out_iterator() = default;
        out_iterator() = default;
        out_iterator(const out_iterator&) = default;
        out_iterator(out_iterator&&) = default;
        out_iterator& operator=(const out_iterator&) = default;
        out_iterator& operator=(out_iterator&&) = default;

        // Regular:
        friend bool operator==(const out_iterator& x, const out_iterator& y)
        {
            if (x.current != y.current) {
                return false;
            }

            if (x.at_parent && y.at_parent) {
                return true;
            } else if (!x.at_parent && !y.at_parent) {
                return x.current_child == y.current_child;
            } else {
                return false;
            }
        }

        friend bool operator!=(const out_iterator& x, const out_iterator& y)
        {
            return !(x == y);
        }

        // Custom constructor.
        out_iterator(bool has_children, bool at_parent, node parent, node current, impl_type current_child) :
            has_children { has_children },
            at_parent { at_parent },
            parent { parent },
            current { current },
            current_child { current_child }
        {}

        // Iterator specific operations.
        out_iterator& operator++()
        {
            if (at_parent) {
                at_parent = false;
            } else {
                ++current_child;
            }
            return *this;
        }

        const out_iterator operator++(int)
        {
            out_iterator copy = *this;
            ++(*this);
            return copy;
        }

        node operator*() const
        {
            if (at_parent) {
                return parent;
            } else {
                return current_child->second;
            }
        }
    };

    struct const_edge_iterator : std::iterator<std::forward_iterator_tag, edge> {

        typedef std::multimap<node, node>::const_iterator impl_type;

        impl_type current;

        // Semiregular:
        ~const_edge_iterator() = default;
        const_edge_iterator() = default;
        const_edge_iterator(const const_edge_iterator&) = default;
        const_edge_iterator(const_edge_iterator&&) = default;
        const_edge_iterator& operator=(const const_edge_iterator&) = default;
        const_edge_iterator& operator=(const_edge_iterator&&) = default;

        // Regular:
        friend bool operator==(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return x.current == y.current;
        }

        friend bool operator!=(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return !(x == y);
        }

        // Custom constructor.
        const_edge_iterator(impl_type current) : current { current } {}

        // Iterator specific operations.
        const_edge_iterator& operator++()
        {
            ++current;
            return *this;
        }

        const const_edge_iterator operator++(int)
        {
            const_edge_iterator copy = *this;
            ++(*this);
            return copy;
        }

        edge operator*() const
        {
            return *current;
        }
    };

    // Semiregular: by default
    // Regular:
    friend bool operator==(const tree& x, const tree& y)
    {
        return x.m_impl == y.m_impl;
    }

    friend bool operator!=(const tree& x, const tree& y)
    {
        return !(x == y);
    }

    // Tree operations:
    void set(const edge& e)
    {
        m_impl.insert(e);
    }

    // Topology operations:
    friend int nodes_count(const tree& t)
    {
        std::vector<node> nodes;

        for (const auto& pr : t.m_impl) {
            nodes.push_back(pr.first);
            nodes.push_back(pr.second);
        }

        std::sort(begin(nodes), end(nodes));

        return std::distance(begin(nodes), std::unique(begin(nodes), end(nodes)));
    }

    friend out_iterator out_begin(const tree& t, node x)
    {
        auto eq_range = t.m_impl.equal_range(x);
        auto children_first = eq_range.first;
        auto children_last = eq_range.second;
        auto parent_edge_it = std::find_if(begin(t.m_impl), end(t.m_impl),
                [x](const std::pair<node, node>& pr) { return pr.second == x; });

        return {
            children_first != children_last,
            true,
            parent_edge_it->first,
            x,
            children_first
        };
    }

    friend out_iterator out_end(const tree& t, node x)
    {
        auto eq_range = t.m_impl.equal_range(x);
        auto children_first = eq_range.first;
        auto children_last = eq_range.second;
        auto parent_edge_it = std::find_if(begin(t.m_impl), end(t.m_impl),
                [x](const std::pair<node, node>& pr) { return pr.second == x; });

        bool has_children = children_first != children_last;

        return {
            has_children,
            false,
            parent_edge_it->first,
            x,
            children_last
        };
    }

    friend const_edge_iterator edge_begin(const tree &t)
    {
        return t.m_impl.begin();
    }

    friend const_edge_iterator edge_end(const tree &t)
    {
        return t.m_impl.end();
    }

};

#endif
