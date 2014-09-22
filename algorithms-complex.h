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

    auto lmm = lagrange_mmetric_adapter<mm_type, 2>(mm);

    path pc = dijkstra(g, cmm, src, dst);
    T dpc = get_metric(dmm, pc);
    if (dpc <= constraint) {
        return pc;
    }

    path pd = dijkstra(g, dmm, src, dst);
    T dpd = get_metric(dmm, pd);
    if (get_metric(dmm, pd) > constraint) {
        return {};
    }

    T cpc = get_metric(cmm, pc);
    T cpd = get_metric(cmm, pd);
    while (get_metric(cmm, pc) != get_metric(cmm, pd)) {

        T lambda = (cpc - cpd) / (dpd - dpc);
        path new_p = dijkstra(g, lmm, src, dst);

        if (get_metric(dmm, new_p) > constraint) {
            pc = new_p;
            cpc = get_metric(cmm, pc);
            dpc = get_metric(dmm, pc);
        } else {
            pd = new_p;
            cpd = get_metric(cmm, pd);
            dpd = get_metric(dmm, pd);
        }
    }

    return dp;
}

#endif
