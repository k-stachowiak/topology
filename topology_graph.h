#ifndef TOPOLOGY_GRAPH_H
#define TOPOLOGY_GRAPH_H

#if 0
concept Graph : Topology {
    void set(edge);
}
#endif

/// Adjacency list implementation of the topological structure.
/// In general this is a sequence of sequences of node identifiers.
/// Therefore if we have an adjacency list object x then x[a] is a sequence
/// of all the nodes that are connected to the node a.
/// This structure is advantageous in case of sparse structures.
struct adj_list {

    std::vector<std::vector<node>> adjacency;

    /// The object enabling iteraton over the topological structure edges.
    /// This type is needed here, because the adjacency list is defined
    /// in terms of nodes and no internal structure represents the edges list.
    struct const_edge_iterator : std::iterator<std::forward_iterator_tag, edge> {

        const adj_list *graph;
        int nd, adj;

        // Custom constructor
        const_edge_iterator(const adj_list *g, int n, int a) : graph { g }, nd { n }, adj { a } {}

        // Semiregular:
        const_edge_iterator() : graph { nullptr }, nd { -1 }, adj { -1 } {}
        const_edge_iterator(const const_edge_iterator&) = default;
        const_edge_iterator(const_edge_iterator&&) = default;
        const_edge_iterator& operator=(const const_edge_iterator&) = default;
        const_edge_iterator& operator=(const_edge_iterator&&) = default;

        // Regular:
        friend bool operator==(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return x.graph == y.graph && x.nd == y.nd && x.adj == y.adj;
        }

        friend bool operator!=(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return !(x == y);
        }

        // Forward Iterator:
        const_edge_iterator& operator++()
        {
            if (++adj == static_cast<int>(graph->adjacency.at(nd).size())) {
				do {
                    ++nd;
				} while (nd != (int)graph->adjacency.size() && !graph->adjacency.at(nd).size());
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
    friend bool operator==(const adj_list& x, const adj_list& y)
    {
        return x.adjacency == y.adjacency;
    }

    friend bool operator!=(const adj_list& x, const adj_list& y)
    {
        return !(x == y);
    }

    // Graph operations:
    void set(const edge& e)
    {
        node from = e.first;
        node to = e.second;
        if (from >= static_cast<node>(adjacency.size())) {
            adjacency.resize(from + 1);
        }
        adjacency[from].push_back(to);
    }

    // Topology operations:
    friend int nodes_count(const adj_list& g)
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

    friend std::vector<node>::const_iterator out_begin(const adj_list& g, node x)
    {
        return g.adjacency[x].begin();
    }

    friend std::vector<node>::const_iterator out_end(const adj_list& g, node x)
    {
        return g.adjacency[x].end();
    }

    friend const_edge_iterator edge_begin(const adj_list& g)
    {
        return { &g, 0, 0 };
    }

    friend const_edge_iterator edge_end(const adj_list& g)
    {
        return { &g, static_cast<node>(g.adjacency.size()), static_cast<node>(0) };
    }
};

/// Adjacency matrix is the implementation of a topological structure that
/// stores the two dimentional array of boolean flags indicating for each
/// position (a, b) whether an edge exists between nodes a and b.
/// This type of implementation gives a terse representation of the structure
/// and is advised for the representation of the dense graphs.
struct adj_matrix {

    std::vector<bool> matrix;
    int nodes;

    /// The iterator enabling iteration over the set of neighbors of the given
    /// node. In the case of the adjacency matrix this type of iterator needs
    /// to traverse a particular row of adjacency matrix skipping the empty
    /// entries.
    struct out_iterator : std::iterator<std::forward_iterator_tag, node> {

        typedef std::vector<bool>::const_iterator impl_type;

        impl_type first, last;
        impl_type current;

        // Semiregular:
		~out_iterator() = default;
        out_iterator() = default;
        out_iterator(const out_iterator&) = default;
        out_iterator(out_iterator&&) = default;
        out_iterator& operator=(const out_iterator&) = default;
        out_iterator& operator=(out_iterator&&) = default;

        // Custom constructor.
        out_iterator(impl_type first, impl_type last) :
            first { first }, last { last }, current { first }
        {}

        // Regular:
        friend bool operator==(const out_iterator& x, const out_iterator& y)
        {
            return x.current == y.current;
        }

        friend bool operator!=(const out_iterator& x, const out_iterator& y)
        {
            return !(x == y);
        }

        // Forward iterator.
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

    /// The iterator enabling visitin all the edges in the given structure.
    struct const_edge_iterator : std::iterator<std::forward_iterator_tag, edge> {

        const adj_matrix *graph;
        int u, v;

        // Semiregular:
        const_edge_iterator() : graph { nullptr }, u { -1 }, v { -1 } {}
        const_edge_iterator(const const_edge_iterator&) = default;
        const_edge_iterator(const_edge_iterator&&) = default;
        const_edge_iterator& operator=(const const_edge_iterator&) = default;
        const_edge_iterator& operator=(const_edge_iterator&&) = default;

        // Custom constructor:
        const_edge_iterator(const adj_matrix *g, int u, int v) : graph { g }, u { u }, v { v } {}

        // Regular:
        friend bool operator==(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return x.graph == y.graph && x.u == y.u && x.v == y.v;
        }

        friend bool operator!=(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return !(x == y);
        }

        // Forward iterator:
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
    adj_matrix() : nodes{ 0 } {}
    ~adj_matrix() = default;
    adj_matrix(const adj_matrix& x) = default;
    adj_matrix(adj_matrix&& x) = default;
    adj_matrix& operator=(const adj_matrix& x) = default;
    adj_matrix& operator=(adj_matrix&& x) = default;

    // Custom constructor:
    template <typename I>
    adj_matrix(I first, I last) : matrix{ first, last }
    {
        double nodes_dbl = sqrt(matrix.size());
        assert((nodes_dbl - (double)(int)nodes_dbl) == 0.0);
        nodes = static_cast<int>(nodes_dbl);
    }

    // Regular:
    friend bool operator==(const adj_matrix& x, const adj_matrix& y)
    {
        return x.nodes == y.nodes && x.matrix == y.matrix;
    }

    friend bool operator!=(const adj_matrix& x, const adj_matrix& y)
    {
        return !(x == y);
    }

    // Graph operations:
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
        std::vector<bool> new_matrix(new_nodes * new_nodes, false);

        for (int f = 0; f < nodes; ++f) {
            for (int t = 0; t < nodes; ++t) {
                new_matrix[new_nodes * f + t] = matrix[nodes * f + t];
            }
        }

        new_matrix[new_nodes * from + to] = true;

        nodes = new_nodes;
        matrix = std::move(new_matrix);
    }

    // Topology operations:
    friend int nodes_count(const adj_matrix& g)
    {
        return g.nodes;
    }

    friend out_iterator out_begin(const adj_matrix& g, node u)
    {
        auto first = begin(g.matrix) + u * g.nodes;
        auto last = first + g.nodes;
        out_iterator result { first, last };

        auto first_true = std::find(first, last, true);
        result.current = first_true;

        return result;
    }

    friend out_iterator out_end(const adj_matrix& g, node u)
    {
        auto last = begin(g.matrix) + u * g.nodes + g.nodes;
        return out_iterator { last, last };
    }

    friend const_edge_iterator edge_begin(const adj_matrix& g)
    {
        const_edge_iterator result { &g, 0, 0 };

        if (!g.matrix.at(0)) {
            ++result;
        }

        return result;
    }

    friend const_edge_iterator edge_end(const adj_matrix& g)
    {
        int N = nodes_count(g);
        return { &g, static_cast<node>(N), static_cast<node>(N) };
    }

};

#endif
