#include <cstdio>

#include <vector>

#include "topology.h"
#include "metrics.h"
#include "algorithms.h"

template <int M>
struct cost_multi_compare {
	bool operator()(const std::array<double, M>& x, const std::array<double, M>& y) {
		return x[0] < y[0];
	}
};

using dummy_multi_metric = multi_metric<double, 2, cost_multi_compare<2>>;

metric_map<double> prepare_metric() {
	metric_map<double> result;
	result.set2(0, 1, 7.0);
	result.set2(0, 2, 9.0);
	result.set2(0, 5, 14.0);
	result.set2(1, 2, 10.0);
	result.set2(1, 3, 15.0);
	result.set2(2, 3, 11.0);
	result.set2(2, 5, 2.0);
	result.set2(3, 4, 6.0);
	result.set2(4, 5, 9.0);
	return result;
}

metric_map<dummy_multi_metric> prepare_multi_metric() {
	metric_map<dummy_multi_metric> result;
	result.set2(0, 1, dummy_multi_metric{ { 7.0, 70.0 } });
	result.set2(0, 2, dummy_multi_metric{ { 9.0, 90.0 } });
	result.set2(0, 5, dummy_multi_metric{ { 14.0, 140.0 } });
	result.set2(1, 2, dummy_multi_metric{ { 10.0, 100.0 } });
	result.set2(1, 3, dummy_multi_metric{ { 15.0, 150.0 } });
	result.set2(2, 3, dummy_multi_metric{ { 11.0, 110.0 } });
	result.set2(2, 5, dummy_multi_metric{ { 2.0, 20.0 } });
	result.set2(3, 4, dummy_multi_metric{ { 6.0, 60.0 } });
	result.set2(4, 5, dummy_multi_metric{ { 9.0, 90.0 } });
	return result;
}

adj_list_graph prepare_adj_lst() {
	std::deque<std::deque<node_id>> adjacency{
		{ 1, 2, 5 },
		{ 0, 2, 3 },
		{ 0, 1, 3, 5 },
		{ 1, 2, 4 },
		{ 3, 5 },
		{ 0, 2, 4 }
	};
	return adj_list_graph{ adjacency };
}

adj_matrix_graph prepare_adj_mat() {
	std::deque<bool> matrix{
		0, 1, 1, 0, 0, 1,
		1, 0, 1, 1, 0, 0,
		1, 1, 0, 1, 0, 1,
		0, 1, 1, 0, 1, 0,
		0, 0, 0, 1, 0, 1,
		1, 0, 1, 0, 1, 0
	};
	return adj_matrix_graph{ begin(matrix), end(matrix) };
}

int main()
{
	auto m = prepare_metric();
	auto g1 = prepare_adj_lst();
	auto g2 = prepare_adj_mat();
	path pd1 = dijkstra(g1, m, 0, 4);
	path pd2 = dijkstra(g2, m, 0, 4);
	path pbf1 = bellman_ford(g1, m, 0, 4);
	path pbf2 = bellman_ford(g2, m, 0, 4);
	bool result = all_equal(pd1, pd2, pbf1, pbf2);
	printf("All paths are %s.\n\n", result ? "equal" : "not equal");

	auto ms = prepare_multi_metric();
	path pdm = dijkstra(g1, ms, 0, 4);
	for (const auto& x : pdm) {
		printf("%d ", x);
	}
	printf("\n\n");

	tree tt = prim(g2, m, 0);
	for_each_edge(tt, [](node_id u, node_id v) {
		printf("%d -> %d\n", u, v);
	});
	printf("\n");

	return 0;
}