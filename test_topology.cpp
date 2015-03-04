#include <cassert>

#include "test_common.h"
#include "metric.h"
#include "weight.h"
#include "algorithms_basic.h"
#include "topology.h"

namespace {

    template <class Graph>
    void fill_example_graph_bi(Graph& g)
    {
        for_each_example_metric_dbl(
            [&g](const edge& e, double) { g.set(e); g.set(reverse(e)); }
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
        adj_matrix adj_mat;
        fill_example_graph_bi(adj_mat);
        assert(adj_mat.matrix == expected_adj_mat);

        std::deque<std::deque<node>> expected_adj_list {
            { 1, 2, 5 },
            { 0, 2, 3 },
            { 0, 1, 3, 5 },
            { 1, 2, 4 },
            { 3, 5 },
            { 0, 2, 4 } };
        adj_list adj_list;
        fill_example_graph_bi(adj_list);
        assert(adj_list.adjacency == expected_adj_list);
    }

	void non_graph_path_finding_test()
	{
		hop_metric<int> m;

		path p { 1, 2, 3, 4, 5 };
		path expected_presult { 4, 3, 2 };
		path presult = dijkstra(p, m, 4, 2);

		assert(expected_presult == presult);

        tree t { {
            { 0, 1 }, { 0, 2 },
            { 1, 3 }, { 1, 4 }, { 2, 5 }, { 2, 6 }
        } };
        path expected_tresult { 6, 2, 0, 1, 3 };
        path tresult = dijkstra(t, m, 6, 3);

        assert(expected_tresult == tresult);
	}

}

void test_topology()
{
    initialization_test();
	non_graph_path_finding_test();
}

