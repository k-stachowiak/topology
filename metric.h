#ifndef METRIC_H
#define METRIC_H

#include <numeric>

#if 0
concept Metric : Regular {
    Weight& operator()(Link);
};
#endif

/// Metric which will return Weight(1) for any Link.
/// @tparam Weight A Weight.
///
template <class Weight>
struct hop_metric {

    // Helper typedef.
	typedef Weight weight_type;

	// Semiregular: by default.

	// Regular:
	friend bool operator==(const hop_metric& x, const hop_metric& y) { return true; }
	friend bool operator!=(const hop_metric& x, const hop_metric& y) { return !(x == y); }

	// Operations:
	weight_type operator()(const link&) const {
		return weight_limits<Weight>::one();
	}
};

/// Metric that enables assigning particular weights to particular links.
///
/// @tparam Weight An underlying Wieght.
/// @bidirectional A flag indicating if the map lookup should be uni- or bi- directional.
///
template <class Weight, bool bidirectional = false>
class map_metric {

    // Implementation
	std::map<link, Weight> m_impl;

public:
	typedef Weight weight_type;

	// Semiregular: by default.

	// Regular:
	friend bool operator==(const map_metric& x, const map_metric& y) { return x.m_impl == y.m_impl; }
	friend bool operator!=(const map_metric& x, const map_metric& y) { return !(x == y); }

	// Operations:
	const weight_type operator()(const link& l) const {

		if (!bidirectional) {
			return m_impl.at(l);
		}

		auto it = m_impl.find(l);
		if (it != end(m_impl)) {
			return it->second;
		} else {
			return m_impl.at(l);
		}
	}

	weight_type& operator()(const link &l) {

		if (!bidirectional) {
			return m_impl[l];
		}

		auto it = m_impl.find(l);
		if (it != end(m_impl)) {
			return it->second;
		} else {
			return m_impl[l];
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
		static const weight_type get_const(const Metric& metric, ConstFunc func, const link& l) {
            const auto& seq = metric(l).m_impl;
            return func(begin(seq), end(seq));
        }

		template <typename Func>
        static weight_type& get(Metric& metric, Func func, const link& l) {
            const auto& seq = metric(l).m_impl;
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
    const weight_type operator()(const link &l) const {
		auto cfunc = [this](metric_const_iterator first, metric_const_iterator) -> const weight_type {
			return *(first + m_index);
		};
		return detail::metric_aggregator<Metric>::get_const(m_impl, cfunc, l);
	}

    weight_type& operator()(const link &l) {
		auto func = [this](metric_iterator first, metric_iterator) -> weight_type& {
			return *(first + m_index);
		};
		return detail::metric_aggregator<Metric>::get(m_impl, func, l);
	}
};

#endif
