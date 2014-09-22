#ifndef METRIC_BASIC_H
#define METRIC_BASIC_H

// concept Metric m
//
// Effectively metric is a function from node pairs to an edge weight.
// operations:
// - (member) get weight : node -> node -> weight (read / write access)

template <class Weight>
// Weight fulfills the weiht concept
struct hop_metric {

	typedef Weight weight_type;

	// Semiregular: by default.
	// Regular:
	friend bool operator==(const hop_metric& x, const hop_metric& y) {
		return true;
	}
	friend bool operator!=(const hop_metric& x, const hop_metric& y) {
		return !(x == y);
	}

	// Operations:
	weight_type operator()(node_id from, node_id to) const {
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
	typedef Weight weight_type;

	// Semiregular: by default.
	// Regular:
	friend bool operator==(const map_metric& x, const map_metric& y) {
		return x.m_impl == y.m_impl;
	}
	friend bool operator!=(const map_metric& x, const map_metric& y) {
		return !(x == y);
	}

	// Operations:
	const weight_type operator()(node_id from, node_id to) const {

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

	weight_type& operator()(node_id from, node_id to) {

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

#endif
