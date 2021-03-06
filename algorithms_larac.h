#ifndef ALGORITHMS_LARAC_H
#define ALGORITHMS_LARAC_H

#include <type_traits>

#include "algorithms_basic.h"

namespace detail {

    template <class Topology, class Metric, class WeightCmp>
    bool larac_equal(const Topology& x, const Topology& y, const Metric& m, const WeightCmp& cmp) {
        const auto wx = accumulate_weight(m, x);
        const auto wy = accumulate_weight(m, y);
        return !cmp(wx, wy) && !cmp(wy, wx);
    }

}

template <class Graph, class Metric>
path larac(const Graph& g, const Metric& m, double constraint, node src, node dst)
{
    using MW = typename Metric::weight_type;

    auto ccmp = weight_cmp_index<MW, 0> {};
    auto dcmp = weight_cmp_index<MW, 1> {};

    // Check for immediate success.
    path pc = dijkstra(g, m, src, dst, ccmp);
    auto pc_weight = accumulate_weight(m, pc);
    double dpc = pc_weight[1];
    if (dpc <= constraint) {
        return pc;
    }

    // Check for immediate failure.
    path pd = dijkstra(g, m, src, dst, dcmp);
    auto pd_weight = accumulate_weight(m, pd);
    double dpd = pd_weight[1];
    if (dpd > constraint) {
        return {};
    }

    // Approximation loop.
    double cpc = pc_weight[0];
    double cpd = pd_weight[0];
    while (true) {

        double lambda = (cpc - cpd) / (dpd - dpc);

        weight_cmp_aggr<MW, weight_aggr_lincmb<MW>> lccmp {{{ 1.0, lambda }}}; // 1 * cost + lambda * delay
        path np = dijkstra(g, m, src, dst, lccmp);
        auto np_weight = accumulate_weight(m, np);

        if (np_weight[1] > constraint) {
            pc = np;
            cpc = np_weight[0];
            dpc = np_weight[1];
        } else {
            pd = np;
            cpd = np_weight[0];
            dpd = np_weight[1];
        }

        if (detail::larac_equal(pc, pd, m, lccmp)) {
            return pd;
        }
    }

    throw std::runtime_error("Should not get here.");
}

#endif
