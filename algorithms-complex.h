#ifndef ALGORITHMS_COMPLEX_H
#define ALGORITHMS_COMPLEX_H

#include "algorithms-basic.h"

template <class Graph, class Metric>
path larac(const Graph& g, const Metric& m, Metric::weight_type constraint, node src, node dst)
{
    using Weight = Metric::weight_type;

    index_metric_adapter<decltype(m)> cm { &m, 0 };
    index_metric_adapter<decltype(m)> dm { &m, 1 };
    lagrange_mmetric_adapter<decltype(m)> lm { &m, 2 };

    path pc = dijkstra(g, cm, src, dst);
    Weight dpc = get_metric(dm, pc);
    if (dpc <= constraint) {
        return pc;
    }

    path pd = dijkstra(g, dm, src, dst);
    Weight dpd = get_metric(dm, pd);
    if (get_metric(dm, pd) > constraint) {
        return {};
    }

    Weight cpc = get_metric(cm, pc);
    Weight cpd = get_metric(cm, pd);
    while (get_metric(cm, pc) != get_metric(cm, pd)) {

        Weight lambda = (cpc - cpd) / (dpd - dpc);
        path new_p = dijkstra(g, lm, src, dst);

        if (get_metric(dm, new_p) > constraint) {
            pc = new_p;
            cpc = get_metric(cm, pc);
            dpc = get_metric(dm, pc);
        } else {
            pd = new_p;
            cpd = get_metric(cm, pd);
            dpd = get_metric(dm, pd);
        }
    }

    return dp;
}

#endif
