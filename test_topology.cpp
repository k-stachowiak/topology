#include <cassert>

#include "test_common.h"
#include "weight.h"
#include "topology.h"

namespace {

    template <class Graph>
    void fill_example_graph_bi(Graph& g)
    {
        for_each_example_edge(
            [&g](const edge& e) { g.set(e); g.set(reverse(e)); }
        );
    }

    void initialization_test()
    {
        std::deque<bool> expected_adj_mat {
            0, 1, 1, 0, 0, 1,
            1, 0, 1, 1, 0, 0,
            1, 1, 0, 1, 0, 1,
            0, 1, 1, 0, 1, 0,
            0, 0, 0, 1, 0, 1,
            1, 0, 1, 0, 1, 0 };
        adj_matrix_graph adj_mat;
        fill_example_graph_bi(adj_mat);
        assert(adj_mat.matrix == expected_adj_mat);

        std::deque<std::deque<node>> expected_adj_list {
            { 1, 2, 5 },
            { 0, 2, 3 },
            { 0, 1, 3, 5 },
            { 1, 2, 4 },
            { 3, 5 },
            { 0, 2, 4 } };
        adj_list_graph adj_list;
        fill_example_graph_bi(adj_list);
        assert(adj_list.adjacency == expected_adj_list);
    }

}

void test_topology()
{
    initialization_test();
}

