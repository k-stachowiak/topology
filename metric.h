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
	weight_type operator()(const edge&) const {
		return weight_limits<Weight>::one();
	}
};

/// Metric that enables assigning particular weights to particular edges.
///
/// @tparam Weight An underlying Wieght.
/// @bidirectional A flag indicating if the map lookup should be uni- or bi- directional.
///
template <class Weight, bool bidirectional = false>
class map_metric {

    // Helper definition of the edge normalization operation.
    static edge normalize(const edge& e) { return std::minmax(e.first, e.second); }

    // Implementation
	std::map<edge, Weight> m_impl;

public:
	typedef Weight weight_type;

	// Semiregular: by default.

	// Regular:
	friend bool operator==(const map_metric& x, const map_metric& y)
    {
        // In any case implementation equality means the metric equality.
        if (x.m_impl == y.m_impl) {
            return true;
        }

        // If not bidirectional then the equality is only given
        // by the implementation equality.
        if (!bidirectional) {
            return false;
        }

        for (const auto& pr : x.m_impl) {

            // Match straight.
            auto found1 = y.m_impl.find(pr.first);
            if (found1 != end(y.m_impl) && pr.second == found1->second) {
                continue;
            }

            // Match reverse
            node from = pr.first.first;
            node to = pr.first.second;
            auto found2 = y.m_impl.find(edge(to, from));
            if (found2 == end(y.m_impl) || pr.second != found2->second) {
                return false;
            }
        }

        return true;
    }

	friend bool operator!=(const map_metric& x, const map_metric& y) { return !(x == y); }

	// Operations:
	const weight_type& operator()(const edge& e) const
    {
        return bidirectional
            ? m_impl.at(normalize(e))
            : m_impl.at(e);
	}

	weight_type& operator()(const edge &e)
    {
        return bidirectional
            ? m_impl[normalize(e)]
            : m_impl[e];
	}
};

/// Metric that allows to only take into account a certain index of the multi-meteic.
///
/// @tparam Metric A Metric based on a MultiWeight type.
template <typename Metric>
struct index_metric_adapter {

	int m_index;
	Metric* m_impl;

	typedef typename Metric::weight_type::iterator metric_iterator;
	typedef typename Metric::weight_type::const_iterator metric_const_iterator;
    typedef typename Metric::weight_type::value_type weight_type;

    // Semiregular:
    index_metric_adapter() = default;
    index_metric_adapter(const index_metric_adapter&) = default;
    index_metric_adapter& operator=(const index_metric_adapter&) = default;
    index_metric_adapter(index_metric_adapter&& x) :
        m_impl { std::move(x.m_impl) },
        m_index { x.m_index }
    {}

    index_metric_adapter& operator=(index_metric_adapter&& x)
    {
        m_impl = std::move(x.m_impl);
        m_index = x.m_index;
        return *this;
    }

	index_metric_adapter(Metric* impl, int index) :
		m_index(index),
		m_impl(impl)
	{}

    // Regular:
	friend bool operator==(const index_metric_adapter& x, const index_metric_adapter& y)
    {
        return x.m_impl->m_impl == y.m_impl->m_impl;
	}

	friend bool operator!=(const index_metric_adapter& x, const index_metric_adapter& y)
    {
		return !(x == y);
	}

	// Operations:
    weight_type operator()(const edge &e) const
    {
        const auto w = (*m_impl)(e);
        return *(w.begin() + m_index);
	}

    weight_type operator()(const edge &e)
    {
        return static_cast<const index_metric_adapter&>(*this)(e);
	}
};

#endif
