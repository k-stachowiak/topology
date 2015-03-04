#ifndef TOPOLOGY_PATH_H
#define TOPOLOGY_PATH_H

#if 0
concept Path : Topology {
    void push_front(node);
    void push_back(node);
}
#endif

using path = std::deque<node>;

struct path_out_iterator : std::iterator<std::forward_iterator_tag, node> {
	enum Phase {
		FIRST, SECOND, END
	} phase = FIRST;
	node first = -1, second = -1;

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

#endif
