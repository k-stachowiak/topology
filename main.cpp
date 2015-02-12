#include <iostream>
#include <fstream>
#include <vector>

#include "topology.h"
#include "weight.h"
#include "metric.h"
#include "algorithms-basic.h"

void test();

// TODO:
// * add optional begin(.) and end(.) iterators to the weight concept.
// * node and edge iterators for topological sturctures.
// todo: split map_weight into directed map_weight = map<pair<node, node>> and undirected - half of the current impl.
// todo: rephrase the weight as an additive, ordered monoid.

/// The architecture:
///
/// Weights:
/// Regular types for measuring the costs of edges, paths, etc.
///
/// Topologies:
/// Types for storing pure topological information, e.g. the node adjacency, but
/// not the edge weights.
///
/// Metrics:
/// Functions from topologies to weights, i.e. means for evaluating
/// the costs of the topological structures of different complexities.
///
/// Algorithms:
/// Transformations between topologies; most commonly graph -> path or
/// grahp -> tree.

using dummy_multi_weight = multi_weight<double, 2, cost_multi_compare<double, 2>>;

adj_list_graph prepare_adj_lst()
{
    /* NOTE: this is missing 4-5 edge compared to adj matrix. */
	std::deque<std::deque<node>> adjacency{
		{ 1, 2, 5 },
		{ 0, 2, 3 },
		{ 0, 1, 3, 5 },
		{ 1, 2, 4 },
		{ 3 },
		{ 0, 2 }
	};
	return adj_list_graph{ adjacency };
}

adj_list_graph prepare_full_adj_lst()
{
    auto result = prepare_adj_lst();
    result.set2(5, 4);
    return result;
}

adj_matrix_graph prepare_adj_mat()
{
    /* NOTE: this is missing 0-2 edge compared to adj list. */
	std::deque<bool> matrix{
		0, 1, 0, 0, 0, 1,
		1, 0, 1, 1, 0, 0,
		0, 1, 0, 1, 0, 1,
		0, 1, 1, 0, 1, 0,
		0, 0, 0, 1, 0, 1,
		1, 0, 1, 0, 1, 0
	};
	return adj_matrix_graph{ begin(matrix), end(matrix) };
}

adj_matrix_graph prepare_full_adj_mat()
{
    auto result = prepare_adj_mat();
    result.set2(0, 2);
    return result;
}

map_metric<double, true> prepare_weight()
{
	map_metric<double, true> result;
	result(edge { 0, 1 }) = 7.0;
	result(edge { 0, 2 }) = 9.0;
	result(edge { 0, 5 }) = 14.0;
	result(edge { 1, 2 }) = 10.0;
	result(edge { 1, 3 }) = 15.0;
	result(edge { 2, 3 }) = 11.0;
	result(edge { 2, 5 }) = 2.0;
	result(edge { 3, 4 }) = 6.0;
	result(edge { 4, 5 }) = 9.0;
	return result;
}

map_metric<dummy_multi_weight, true> prepare_multi_weight()
{
	map_metric<dummy_multi_weight, true> result;
	result(edge { 0, 1 }) = dummy_multi_weight{ 7.0, 70.0 };
	result(edge { 0, 2 }) = dummy_multi_weight{ 9.0, 90.0 };
	result(edge { 0, 5 }) = dummy_multi_weight{ 14.0, 140.0 };
	result(edge { 1, 2 }) = dummy_multi_weight{ 10.0, 100.0 };
	result(edge { 1, 3 }) = dummy_multi_weight{ 15.0, 150.0 };
	result(edge { 2, 3 }) = dummy_multi_weight{ 11.0, 110.0 };
	result(edge { 2, 5 }) = dummy_multi_weight{ 2.0, 20.0 };
	result(edge { 3, 4 }) = dummy_multi_weight{ 6.0, 60.0 };
	result(edge { 4, 5 }) = dummy_multi_weight{ 9.0, 90.0 };
	return result;
}

template <class MetricMap>
void print(const path& p, const MetricMap& mm)
{
    for (const auto& n : p) {
        std::cout << n << ", ";
    }
    
    auto cost = weight_limits<typename MetricMap::weight_type>::zero();
    for_each_edge(p, [&cost, &mm](const edge& e) { // TODO: implement node and edge iterators for topological structures.
        cost += mm(e);
    });

    std::cout << " -> " << cost << std::endl;
}

template <class MetricMap>
void print(const tree& t, const MetricMap& mm)
{
	for_each_edge(t, [](node u, node v) {
        std::cout << u << " - " << v << " -> ";
	});
    std::cout << std::endl;
}

void single_weight_test()
{
    std::cout << "Single weight test." << std::endl;

	auto m = prepare_weight();
	auto g1 = prepare_full_adj_lst();
	auto g2 = prepare_full_adj_mat();

	path pd1 = dijkstra(g1, m, 0, 4);
    print(pd1, m);

	path pd2 = dijkstra(g2, m, 0, 4);
    print(pd2, m);

	path pbf1 = bellman_ford(g1, m, 0, 4);
    print(pbf1, m);

	path pbf2 = bellman_ford(g2, m, 0, 4);
    print(pbf2, m);
    
	bool result = all_equal(pd1, pd2, pbf1, pbf2);
    std::cout << "All paths are " << (result ? "equal" : "not equal") <<
        std::endl << std::endl;
}

void multi_weight_test()
{
    std::cout << "Multi weight test." << std::endl;

	auto m = prepare_weight();

	auto g1 = prepare_full_adj_lst();
	auto g2 = prepare_full_adj_mat();

	auto ms = prepare_multi_weight();
	path pdm = dijkstra(g1, ms, 0, 4);
    print(pdm, m);

	tree t = prim(g2, m, 0);

    std::cout << std::endl;
}

void hop_metric_test()
{
    std::cout << "Hop-metric test." << std::endl;

    auto g = prepare_full_adj_mat();

    auto dm = prepare_weight();
    hop_metric<double> hm;

    path dp = dijkstra(g, dm, 0, 5);
    path hp = dijkstra(g, hm, 0, 5);

    std::cout << "Distance path = ";
    print(dp, dm);
    std::cout << "Hop path = ";
    print(hp, hm);

    std::cout << std::endl;
}

void index_metric_test()
{
    std::cout << "Index metric test." << std::endl;

	auto g = prepare_full_adj_mat();
    auto m = prepare_multi_weight();

    index_metric_adapter<decltype(m)> m0 { &m, 0 };
    index_metric_adapter<decltype(m)> m1 { &m, 1 };

    path p0 = dijkstra(g, m0, 0, 5);
    path p1 = dijkstra(g, m1, 0, 5);

    std::cout << "Path for metric 0 = ";
    print(p0, m0);
    std::cout << "Path for metric 1 = ";
    print(p1, m1);
}

struct std_line : std::string {
    friend std::istream& operator>>(std::istream& in, std_line& line) {
        return std::getline(in, line);
    }
};

void file_iteration()
{
    std::ifstream in { "graph" };

    std::istream_iterator<std_line> first(in);
    std::istream_iterator<std_line> last;

    std::ostream_iterator<std::string> out(std::cout, "\n");

    std::copy(first, last, out);
}

int main()
{
    test();
	return 0;
}

