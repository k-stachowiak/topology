#ifndef ALGORITHMS_MLRA_H
#define ALGORITHMS_MLRA_H

#include "algorithms_larac.h"

namespace detail {

    // void mlra_delete_leaves(...) { This is probably very rarely needed; if ever... }

}

template <class Graph, class Metric, class NodeIt>
tree mlra(const Graph& g, const Metric& m, double constraint, node src, NodeIt dst_begin, NodeIt dst_end)
{
    using Weight = typename Metric::weight_type;

    tree result;

    while (dst_begin != dst_end) {
        const auto& dst = *dst_begin++;
        path p = larac(g, m, constraint, src, dst);
        if (p.empty()) {
            return {};
        } else {
            std::for_each(edge_begin(p), edge_end(p), [&result](const edge& e) {
                result.set(e);
				result.set(reverse(e));
            });
        }
    }

    result = prim(result, m, src, weight_cmp_cost<Weight> {});
    // result = detail::mlra_delete_leaves(result, dst_begin, dst_end);

    return result;
}

#endif
