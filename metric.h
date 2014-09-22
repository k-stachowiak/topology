#ifndef METRIC_MAP_H
#define METRIC_MAP_H

// concept Metric m
//
// Effectively metric is a function from node pairs to an edge weight.
// operations:
// - (member) get weight : node -> node -> weight (read / write access)


template <class Weight>
// Weight fulfills the weiht concept
struct hop_metric {

	typedef Weight value_type;

	// Semiregular: by default.
	// Regular:
	friend bool operator==(const hop_metric& x, const hop_metric& y) {
		return true;
	}
	friend bool operator!=(const hop_metric& x, const hop_metric& y) {
		return !(x == y);
	}

	// Operations:
	Weight operator()(node_id from, node_id to) const {
		return weight_limits<Weight>::one();
	}
};

template <class Weight, bool bidirectional = false>
// Weight fulfills the weiht concept
class map_metric {

	// This type enables storing a map between edges and their metrics.
	// It will be required for the optimization algorithms that interpret
	// the edge weights.

	std::map<std::pair<node_id, node_id>, Weight> m_impl;

public:
	typedef Weight value_type;

	// Semiregular: by default.
	// Regular:
	friend bool operator==(const map_metric& x, const map_metric& y) {
		return x.m_impl == y.m_impl;
	}
	friend bool operator!=(const map_metric& x, const map_metric& y) {
		return !(x == y);
	}

	// Operations:
	const Weight operator()(node_id from, node_id to) const {

		if (!bidirectional) {
			return m_impl.at(std::make_pair(to, from));
		}

		auto it = m_impl.find(std::make_pair(from, to));
		if (it != end(m_impl)) {
			return it->second;
		} else {
			return m_impl.at(std::make_pair(to, from));
		}
	}

	Weight& operator()(node_id from, node_id to) {

		if (!bidirectional) {
			return m_impl[std::make_pair(from, to)];
		}

		auto it = m_impl.find(std::make_pair(from, to));
		if (it != end(m_impl)) {
			return it->second;
		} else {
			return m_impl[std::make_pair(from, to)];
		}
	}
};

/*
template <int I, typename T, int M, typename Cmp = std::less<T>>
class index_mm_adapter {

    metric_map<multi_weight<T, M, Cmp>> m_impl;

public:
    // Semiregular: by default.
    // Regular:
	friend bool operator==(const index_mm_adapter& x, const index_mm_adapter& y) {
		return x.m_impl.m_impl.at(I) == y.m_impl.m_impl.at(I);
	}
	friend bool operator!=(const index_mm_adapter& x, const index_mm_adapter& y) {
		return !(x == y);
	}

	// Operations:
	void set(node_id from, node_id to, const T& m) {
		m_impl.m_impl[std::make_pair(from, to)][I] = m;
	}

	void set2(node_id from, node_id to, const T& m) {
		set(from, to, m);
		set(to, from, m);
	}

	T operator()(node_id from, node_id to) const {
		return m_impl.m_impl.at(std::make_pair(from, to)).at(I);
	}
};
*/

// Weight aggregation:
//
// This idea consists in being able to map topological structures to 
// metric values:
//
// path -> metric_map -> metric
// tree -> metric_map -> metric
// ...

// Basic metric aggregation.
template <class Metric>
typename Metric::value_type get_weight(const path& p, const Metric& m) {

    auto first = p.begin();
    auto last = p.end();

    typename Metric::value_type result = weight_limits<
        typename Metric::value_type>::zero();

    while (first + 1 != last) {
        auto next = first + 1;
        result = result + m(*first, *next);
        first = next;
    }

    return result;
}

#endif
