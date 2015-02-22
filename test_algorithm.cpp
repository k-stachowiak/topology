#include "test_common.h"
#include "metric.h"
#include "weight_cmp.h"
#include "topology.h"

#include "algorithms_basic.h"
#include "algorithms_larac.h"

namespace {

    template <class Func>
    void for_each_mpiech_edge(Func f)
    {
        f(edge { 0, 4 });
        f(edge { 0, 6 });
        f(edge { 1, 7 });
        f(edge { 1, 6 });
        f(edge { 2, 1 });
        f(edge { 2, 0 });
        f(edge { 3, 2 });
        f(edge { 3, 1 });
        f(edge { 4, 2 });
        f(edge { 4, 3 });
        f(edge { 5, 0 });
        f(edge { 5, 1 });
        f(edge { 6, 4 });
        f(edge { 6, 2 });
        f(edge { 7, 5 });
        f(edge { 7, 4 });
    }

    template <class Func>
    void for_each_mpiech_weight(Func f)
    {
        f(edge { 0, 4 }, array_weight<double, 2> { 394.0, 332.0 });
        f(edge { 0, 6 }, array_weight<double, 2> { 196.0, 346.0 });
        f(edge { 1, 7 }, array_weight<double, 2> { 461.0, 991.0 });
        f(edge { 1, 6 }, array_weight<double, 2> { 445.0, 598.0 });
        f(edge { 2, 1 }, array_weight<double, 2> { 354.0, 469.0 });
        f(edge { 2, 0 }, array_weight<double, 2> { 313.0, 159.0 });
        f(edge { 3, 2 }, array_weight<double, 2> { 417.0, 697.0 });
        f(edge { 3, 1 }, array_weight<double, 2> { 170.0, 1082.0 });
        f(edge { 4, 2 }, array_weight<double, 2> { 787.0, 254.0 });
        f(edge { 4, 3 }, array_weight<double, 2> { 107.0, 443.0 });
        f(edge { 5, 0 }, array_weight<double, 2> { 33.0, 1014.0 });
        f(edge { 5, 1 }, array_weight<double, 2> { 299.0, 771.0 });
        f(edge { 6, 4 }, array_weight<double, 2> { 108.0, 93.0 });
        f(edge { 6, 2 }, array_weight<double, 2> { 347.0, 227.0 });
        f(edge { 7, 5 }, array_weight<double, 2> { 402.0, 570.0 });
        f(edge { 7, 4 }, array_weight<double, 2> { 199.0, 561.0 });
    }

    void test_larac()
    {
        // Expected runtime scenario:
        //
        // c0 = 435  - cost
        // d0 = 1584 - delay
        // 
        // l1 = 0,228   - lambda
        // p1 = (0 5 7) - path
        // c1 = 435
        // d1 = 1584
        // 
        // l2 = 0,116
        // p2 = (0 6 4 7)
        // c2 = 503
        // d2 = 1000

        using weight_type = array_weight<double, 2>;

        adj_matrix_graph g;
        for_each_mpiech_edge([&g](const edge& e) { g.set(e); g.set(reverse(e)); });

        map_metric<weight_type, true> m;
        for_each_mpiech_weight([&m](const edge& e, const weight_type& w) { m(e) = w; });

        path expected_p { 0, 6, 4, 7 };
        path p = larac(g, m, 1000.0, 0, 7);

        assert(p == expected_p);
    }

    template <class Graph>
    void prepare_wiki_graph(Graph& g)
    {
        for_each_example_edge([&g](const edge& e) {
            g.set(e); g.set(reverse(e));
        });
    }

    void test_simple()
    {
        adj_list_graph g;
        prepare_wiki_graph(g);

        map_metric<double, true> m;
        for_each_example_metric_dbl([&m](const edge& e, double val) { m(e) = val; });

        path expected_p { 0, 2, 5, 4 };

        path pd = dijkstra(g, m, 0, 4);
        path pb = bellman_ford(g, m, 0, 4);

        assert(pd == expected_p);
        assert(pb == expected_p);
    }

    void test_multi()
    {
        using weight_type = array_weight<double, 2>;

        adj_matrix_graph g;
        prepare_wiki_graph(g);

        map_metric<weight_type, true> m;
        for_each_example_metric_dbl([&m](const edge& e, double val) {
            m(e) = weight_type { 10 * val, val };
        });

        path expected_p { 0, 2, 5, 4 };

        path pd = dijkstra(g, m, 0, 4, weight_cmp_cost<weight_type> {});
        path pb = bellman_ford(g, m, 0, 4, weight_cmp_cost<weight_type> {});

        assert(pd == expected_p);
        assert(pb == expected_p);
    }

    void test_hop()
    {
        adj_list_graph g;
        prepare_wiki_graph(g);

        hop_metric<double> m;

        path pd = dijkstra(g, m, 0, 3);
        path pb = bellman_ford(g, m, 0, 3);

        assert(nodes_count(pd) == 3);
        assert(nodes_count(pb) == 3);
    }

}

void test_algorithm()
{
    // Dijkstra / Bellman-Ford algorithms test.
    test_simple();
    test_multi();
    test_hop();

    // Test custom algorithms.
    test_larac();
}
