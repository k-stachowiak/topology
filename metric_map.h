#ifndef METRIC_MAP_H
#define METRIC_MAP_H

// concept MetricMap mm
//
// operations:
// - (member) set metric undirected : node -> node -> metric -> () ("set")
// - (member) set metric directed : node -> node -> metric -> () ("set2")
// - (member) get metric : node -> node -> metric

template <class Metric>
struct hop_map {

	typedef Metric value_type;

	// Semiregular: by default.
	// Regular:
	friend bool operator==(const hop_map& x, const hop_map& y) {
		return true;
	}
	friend bool operator!=(const hop_map& x, const hop_map& y) {
		return !(x == y);
	}

	// Operations:
	void set(node_id, node_id, const Metric&) {}
	void set2(node_id, node_id, const Metric&) {}

	Metric operator()(node_id from, node_id to) const {
		return metric_limits<Metric>::one();
	}
};

template <class Metric>
// Metric fulfills the Metric concept
class metric_map {

	// This type enables storing a map between edges and their metrics.
	// It will be required for the optimization algorithms that interpret
	// the edge weights.

	std::map<std::pair<node_id, node_id>, Metric> m_impl;
public:

	typedef Metric value_type;

	// Semiregular: by default.
	// Regular:
	friend bool operator==(const metric_map& x, const metric_map& y) {
		return x.m_impl == y.m_impl;
	}
	friend bool operator!=(const metric_map& x, const metric_map& y) {
		return !(x == y);
	}

	// Operations:
	void set(node_id from, node_id to, const Metric& m) {
		m_impl[std::make_pair(from, to)] = m;
	}

	void set2(node_id from, node_id to, const Metric& m) {
		set(from, to, m);
		set(to, from, m);
	}

	Metric operator()(node_id from, node_id to) const {
		return m_impl.at(std::make_pair(from, to));
	}
};

template <int I, typename T, int M, typename Cmp = std::less<T>>
class index_mm_adapter {

    metric_map<multi_metric<T, M, Cmp>> m_impl;

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

// Metric aggregation:
//
// This idea consists in being able to map topological structures to 
// metric values:
//
// path -> metric_map -> metric
// tree -> metric_map -> metric
// ...

// Basic metric aggregation.
template <class MetricMap>
typename MetricMap::value_type get_metric(const path& p, const MetricMap& mm) {

    auto first = p.begin();
    auto last = p.end();

    typename MetricMap::value_type result = metric_limits<
        typename MetricMap::value_type>::zero();

    while (first + 1 != last) {
        auto next = first + 1;
        result = result + mm(*first, *next);
        first = next;
    }

    return result;
}


#endif
