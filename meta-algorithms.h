#ifndef META_ALGORITHMS_H
#define META_ALGORITHMS_H

#include "algorighms.h"

template <class Graph, typename T>
path larac(const Graph& g,
           const metric_map<multi_metric<T, 2, std::less<T>>>& mm,
           T constraint,
           node_id src, node_id dst) {

    using mm_type = std::remove_const<std::remove_reference<
            decltype(mm)>::type>::type;

    auto cmm = index_mmetric_adapter<mm_type, 0>(mm);
    auto dmm = index_mmetric_adapter<mm_type, 1>(mm);
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
