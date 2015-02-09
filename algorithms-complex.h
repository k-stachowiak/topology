#ifndef ALGORITHMS_COMPLEX_H
#define ALGORITHMS_COMPLEX_H

#include "algorighms-basic.h"

template <class Graph, typename T>
path larac(const Graph& g,
           const map_metric<multi_weight<T, 2>>& mm,
           T constraint,
           node_id src, node_id dst) {

    static_index_metric_adapter<decltype(mm), 0> cm { mm };
    static_index_metric_adapter<decltype(mm), 1> dm { mm };
    auto lm = lagrange_mmetric_adapter<mm_type, 2>(mm);

    path pc = dijkstra(g, cm, src, dst);
    T dpc = get_metric(dm, pc);
    if (dpc <= constraint) {
        return pc;
    }

    path pd = dijkstra(g, dm, src, dst);
    T dpd = get_metric(dm, pd);
    if (get_metric(dm, pd) > constraint) {
        return {};
    }

    T cpc = get_metric(cm, pc);
    T cpd = get_metric(cm, pd);
    while (get_metric(cm, pc) != get_metric(cm, pd)) {

        T lambda = (cpc - cpd) / (dpd - dpc);
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
