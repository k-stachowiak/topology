#ifndef METRIC_H
#define METRIC_H

#include <numeric>

#include "config.h"
#include "weight.h"

#if 0

concept Metric<Weight> : Regular {
    Weight& operator()(edge);
};

#endif

/// Metric which will return Weight(1) for any Link.
/// @tparam Weight A Weight.
template <Weight W>
struct hop_metric {

    typedef W weight_type;

    // Semiregular: by default.

    // Regular:
    friend bool operator==(const hop_metric& x, const hop_metric& y) { return true; }
    friend bool operator!=(const hop_metric& x, const hop_metric& y) { return !(x == y); }

    // Metric operations:
    weight_type operator()(const edge&) const
    {
        return weight_traits<W>::one();
    }
};

/// Metric that enables assigning particular weights to particular edges.
///
/// @tparam Weight An underlying Wieght.
/// @bidirectional A flag indicating if the map lookup should be uni- or bi- directional.
template <Weight W, bool bidirectional = false>
class map_metric {

    std::map<edge, W> m_impl;

public:
    typedef W weight_type;

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

    // Metric operations:
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

#endif
