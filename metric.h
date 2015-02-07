#ifndef METRIC_H
#define METRIC_H

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

namespace detail {

    template <typename Metric>
    // Metric is based on the mutli_weight,
    struct metric_aggregator {

        typedef typename std::remove_const<
            typename std::remove_reference<
                typename Metric::weight_type::value_type
            >::type
        >::type weight_type;

        // Operations:
		template <typename ConstFunc>
		static const weight_type get_const(
				const Metric& metric,
				ConstFunc func,
				node_id from,
				node_id to) {
            const auto& seq = metric(from, to).m_impl;
            return func(begin(seq), end(seq));
        }

		template <typename Func>
        static weight_type& get(
				Metric& metric,
				Func func,
				node_id from,
				node_id to) {
            const auto& seq = metric(from, to).m_impl;
            return func(begin(seq), end(seq));
        }
    };

}

template <typename Metric>
struct index_metric_adapter {

	int m_index;
	Metric m_impl;

	typedef typename Metric::weight_type::iterator metric_iterator;
	typedef typename Metric::weight_type::const_iterator metric_const_iterator;
    typedef typename decltype(m_impl)::weight_type::value_type weight_type;

    // Semiregular:
    index_metric_adapter() = default;
    index_metric_adapter(const index_metric_adapter&) = default;
    index_metric_adapter& operator=(const index_metric_adapter&) = default;

    index_metric_adapter(index_metric_adapter&& x) :
        m_impl { std::move(x.m_impl) },
        m_index { x.m_index }
    {}

    index_metric_adapter& operator=(index_metric_adapter&& x) {
        m_impl = std::move(x.m_impl);
        m_index = x.m_index;
        return *this;
    }

	index_metric_adapter(const Metric& impl, int index) :
		m_index(index),
		m_impl(impl)
	{}

    // Regular:
	friend bool operator==(const index_metric_adapter& x, const index_metric_adapter& y) {
        return x.m_impl.m_impl == y.m_impl.m_impl; // TODO: Compare only the i-th submetrics?
	}

	friend bool operator!=(const index_metric_adapter& x, const index_metric_adapter& y) {
		return !(x == y);
	}

	// Operations:
    const weight_type operator()(node_id from, node_id to) const {
		auto cfunc = [this](metric_const_iterator first, metric_const_iterator) -> const weight_type {
			return *(first + m_index);
		};
		return detail::metric_aggregator<Metric>::get_const(m_impl, cfunc, from, to);
	}

    weight_type& operator()(node_id from, node_id to) {
		auto func = [this](metric_iterator first, metric_iterator) -> weight_type& {
			return *(first + m_index);
		};
		return detail::metric_aggregator<Metric>::get(m_impl, func, from, to);
	}
};

#endif
