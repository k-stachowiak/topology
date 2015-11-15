#include "test_common.h"
#include "metric.h"
#include "weight_util.h"
#include "topology.h"

#include "algorithms_basic.h"
#include "algorithms_larac.h"
#include "algorithms_mlra.h"
#include "algorithms_lbpsa.h"

namespace {

    template <class Graph, class Metric, Weight W>
    void fill_grid(
            Graph& g, Metric& m,
            int width, int height,
            const W& cheap_weight,
            const W& exp_weight,
            const std::vector<edge>& cheap_edges)
    {
        // Edges + expensive weights.
        // --------------------------
        auto coords_to_node = [width] (int x, int y) -> node { return y * width + x; };

        // o -- o
        // | \_
        // |  \_
        // o    o
        for (int w = 0; w < (width - 1); ++w) {
            for (int h = 0; h < (height - 1); ++h) {
                edge e1 = { coords_to_node(w, h), coords_to_node(w + 1, h) };
                edge e2 = { coords_to_node(w, h), coords_to_node(w, h + 1) };
                edge e3 = { coords_to_node(w, h), coords_to_node(w + 1, h + 1) };

                g.set(e1); g.set(reverse(e1));
                g.set(e2); g.set(reverse(e2));
                g.set(e3); g.set(reverse(e3));

                m(e1) = exp_weight; m(reverse(e1)) = exp_weight;
                m(e2) = exp_weight; m(reverse(e2)) = exp_weight;
                m(e3) = exp_weight; m(reverse(e3)) = exp_weight;
            }
        }

        // o    o
        //    /
        //   /
        // o    o
        for (int w = 0; w < (width - 1); ++w) {
            for (int h = 1; h < height; ++h) {
                edge e = { coords_to_node(w, h), coords_to_node(w + 1, h - 1) };

                g.set(e);
                g.set(reverse(e));

                m(e) = exp_weight;
                m(reverse(e)) = exp_weight;
            }
        }

        // o    o
        //
        //
        // o -- o (lower edges)
        for (int w = 0; w < (width - 1); ++w) {
            edge e = { coords_to_node(w, height - 1), coords_to_node(w + 1, height - 1) };

            g.set(e);
            g.set(reverse(e));

            m(e) = exp_weight;
            m(reverse(e)) = exp_weight;
        }

        // o    o (right edges)
        //      |
        //      |
        // o    o
        for (int w = 0; w < (width - 1); ++w) {
            edge e = { coords_to_node(w, height - 1), coords_to_node(w + 1, height - 1) };

            g.set(e);
            g.set(reverse(e));

            m(e) = exp_weight;
            m(reverse(e)) = exp_weight;
        }

        // Cheap weights.
        // --------------
        for (const edge& e : cheap_edges) {
            m(e) = cheap_weight;
            m(reverse(e)) = cheap_weight;
        }
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

    template <class Graph>
    void prepare_wiki_graph(Graph& g)
    {
        for_each_example_metric_dbl([&g](const edge& e, double) {
            g.set(e);
            g.set(reverse(e));
        });
    }

    void test_mlra()
    {
        using W = array_weight<double, 2>;

        adj_list g;
        map_metric<W> m;
        fill_grid(
            g, m, 3, 3, W { 1.0, 10.0 }, W { 100.0, 1000.0 },
            { { 3, 4 }, { 4, 2 }, { 4, 5 }, { 4, 8 } });

        node src = 3;
        std::vector<node> dst { 2, 5, 8 };
        tree t = mlra(g, m, 10000, src, begin(dst), end(dst));
        tree expected_tree {{
            { 3, 4 }, { 4, 2 }, { 4, 5 }, { 4, 8 }
        }};

        assert(t == expected_tree);
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

        using W = array_weight<double, 2>;

        adj_matrix g;
        map_metric<W, true> m;

        for_each_mpiech_weight([&g, &m](const edge& e, const W& w) {
            g.set(e); g.set(reverse(e));
            m(e) = w; m(reverse(e)) = w;
        });

        path expected_p { 0, 6, 4, 7 };
        path p = larac(g, m, 1000.0, 0, 7);

        assert(p == expected_p);
    }

    void test_simple()
    {
        adj_list g;
        prepare_wiki_graph(g);

        map_metric<double, true> m;
        for_each_example_metric_dbl([&m](const edge& e, double val) { m(e) = val; m(reverse(e)) = val; });

        path expected_p { 0, 2, 5, 4 };

        path pd = dijkstra(g, m, 0, 4);
        path pb = bellman_ford(g, m, 0, 4);

        assert(pd == expected_p);
        assert(pb == expected_p);
    }

    void test_multi()
    {
        using W = array_weight<double, 2>;

        adj_matrix g;
        prepare_wiki_graph(g);

        map_metric<W, true> m;
        for_each_example_metric_dbl([&m](const edge& e, double val) {
            m(e) = W { 10 * val, val };
            m(reverse(e)) = W { 10 * val, val };
        });

        path expected_p { 0, 2, 5, 4 };

        path pd = dijkstra(g, m, 0, 4, weight_cmp_cost<W> {});
        path pb = bellman_ford(g, m, 0, 4, weight_cmp_cost<W> {});

        assert(pd == expected_p);
        assert(pb == expected_p);
    }

    void test_hop()
    {
        adj_list g;
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
    test_mlra();
}
