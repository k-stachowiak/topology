#ifndef TOPOLOGY_TREE_H
#define TOPOLOGY_TREE_H

#if 0
concept Tree : Topology {
    void set(edge);
}
#endif

/// The structure representin a tree, i.e. a topological structure in which
/// between each pair of nodes exists exactly one path.
struct tree {

    std::multimap<node, node> m_impl;

    /// The iterator enabling visiting all the neighbors of the given node.
    struct out_iterator : std::iterator<std::forward_iterator_tag, node> {

        typedef std::multimap<node, node>::const_iterator impl_type;

        bool has_children = false;
        bool at_parent = false;
        node parent = -1;
        node current = -1;
        impl_type current_child;

        // Semiregular:
        ~out_iterator() = default;
        out_iterator() = default;
        out_iterator(const out_iterator&) = default;
        out_iterator(out_iterator&&) = default;
        out_iterator& operator=(const out_iterator&) = default;
        out_iterator& operator=(out_iterator&&) = default;

        // Custom constructor.
        out_iterator(bool has_children, bool at_parent, node parent, node current, impl_type current_child) :
            has_children { has_children },
            at_parent { at_parent },
            parent { parent },
            current { current },
            current_child { current_child }
        {}

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

        // Forward iterator:
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

    /// Iterator enabling traversing all the edges in a given structure.
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

        // Custom constructor.
        const_edge_iterator(impl_type current) : current { current } {}

        // Regular:
        friend bool operator==(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return x.current == y.current;
        }

        friend bool operator!=(const const_edge_iterator& x, const const_edge_iterator& y)
        {
            return !(x == y);
        }

        // Forward iterator:
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
