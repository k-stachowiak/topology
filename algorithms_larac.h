#ifndef ALGORITHMS_COMPLEX_H
#define ALGORITHMS_COMPLEX_H

#include "algorithms_basic.h"

template <class Graph, class Metric>
path larac(
        const Graph& g, const Metric& m,
        typename Metric::weight_type constraint,
        node src, node dst)
{
    using Weight = typename Metric::weight_type;

    index_metric_adapter<decltype(m)> cm { &m, 0 };
    index_metric_adapter<decltype(m)> dm { &m, 1 };

    // Check for immediate success.
    path pc = dijkstra(g, cm, src, dst);
    Weight dpc = metric_add(pc, dm);
    if (dpc <= constraint) {
        return pc;
    }

    // Check for immediate failure.
    path pd = dijkstra(g, dm, src, dst);
    Weight dpd = metric_add(pd, dm);
    if (dpd > constraint) {
        return {};
    }

    // Approximation loop.
    Weight cpc = metric_add(pc, cm);
    Weight cpd = metric_add(pd, cm);
    while (cpc != cpd) {

        Weight lambda = (cpc - cpd) / (dpd - dpc);
        lagrange_metric_adapter<decltype(m)> lm { &m, { lambda }, 2 };

        path new_p = dijkstra(g, lm, src, dst);

        if (metric_add(dm, new_p) > constraint) {
            pc = new_p;
            cpc = metric_add(pc, cm);
            dpc = metric_add(pc, dm);
        } else {
            pd = new_p;
            cpd = metric_add(pd, cm);
            dpd = metric_add(pd, dm);
        }
    }

    return dp;
}

#endif
