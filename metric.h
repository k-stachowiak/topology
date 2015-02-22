#ifndef METRIC_H
#define METRIC_H

#include <numeric>

#include "config.h"
#include "weight.h"

#if 0

concept Metric : Regular {
    Weight& operator()(edge);
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
	weight_type operator()(const edge&) const
    {
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

	Metric* m_impl;
	int m_index;

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

    // Custom constructor
	index_metric_adapter(Metric* impl, int index) :
		m_impl(impl),
		m_index(index)
	{}

    // Regular:
	friend bool operator==(const index_metric_adapter& x, const index_metric_adapter& y)
    {
        return *x.m_impl->m_impl == *y.m_impl->m_impl;
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
};

template <typename Metric>
struct lagrange_metric_adapter {

	typedef typename Metric::weight_type::iterator metric_iterator;
	typedef typename Metric::weight_type::const_iterator metric_const_iterator;
    typedef typename Metric::weight_type::value_type weight_type;

    Metric *m_impl;
    std::vector<weight_type> m_coeff;
    
    // Semiregular:
    lagrange_metric_adapter() = default;
    lagrange_metric_adapter(const lagrange_metric_adapter&) = default;
    lagrange_metric_adapter& operator=(const lagrange_metric_adapter&) = default;

    lagrange_metric_adapter(lagrange_metric_adapter&& x) :
        m_impl { x.m_impl },
        m_coeff { std::move(x.m_coeff) }
    {}

    lagrange_metric_adapter& operator=(lagrange_metric_adapter&& x)
    {
        m_impl = x.m_impl;
        m_coeff = std::move(x.m_coeff);
        return *this;
    }

    // Custom constructor:
    lagrange_metric_adapter(Metric *impl, std::vector<weight_type> coeff) :
        m_impl { impl },
        m_coeff { coeff }
    {}

    // Regular:
    friend bool operator==(const lagrange_metric_adapter& x, const lagrange_metric_adapter& y)
    {
        return
            *x.m_impl->m_impl == *y.m_impl->m_impl &&
            x.m_coeff == y.m_coeff;
    }

    friend bool operator !=(const lagrange_metric_adapter& x, const lagrange_metric_adapter& y)
    {
        return !(x == y);
    }

    // Operations:
    weight_type operator()(const edge &e) const
    {
    }
};

#endif
