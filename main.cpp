#include <iostream>
#include <fstream>
#include <vector>

#include "topology.h"
#include "weight.h"
#include "metric.h"
#include "algorithms.h"

template <int M>
struct cost_multi_compare {
	bool operator()(const std::array<double, M>& x, const std::array<double, M>& y) {
		return x[0] < y[0];
	}
};

using dummy_multi_weight = multi_weight<double, 2, cost_multi_compare<2>>;

metric<double, true> prepare_weight() {
	metric<double, true> result;
	result(0, 1) = 7.0;
	result(0, 2) = 9.0;
	result(0, 5) = 14.0;
	result(1, 2) = 10.0;
	result(1, 3) = 15.0;
	result(2, 3) = 11.0;
	result(2, 5) = 2.0;
	result(3, 4) = 6.0;
	result(4, 5) = 9.0;
	return result;
}

metric<dummy_multi_weight, true> prepare_multi_weight() {
	metric<dummy_multi_weight, true> result;
	result(0, 1) = dummy_multi_weight{ { { 7.0, 70.0 } } };
	result(0, 2) = dummy_multi_weight{ { { 9.0, 90.0 } } };
	result(0, 5) = dummy_multi_weight{ { { 14.0, 140.0 } } };
	result(1, 2) = dummy_multi_weight{ { { 10.0, 100.0 } } };
	result(1, 3) = dummy_multi_weight{ { { 15.0, 150.0 } } };
	result(2, 3) = dummy_multi_weight{ { { 11.0, 110.0 } } };
	result(2, 5) = dummy_multi_weight{ { { 2.0, 20.0 } } };
	result(3, 4) = dummy_multi_weight{ { { 6.0, 60.0 } } };
	result(4, 5) = dummy_multi_weight{ { { 9.0, 90.0 } } };
	return result;
}

adj_list_graph prepare_adj_lst() {
    /* NOTE: this is missing 4-5 edge compared to adj matrix. */
	std::deque<std::deque<node_id>> adjacency{
		{ 1, 2, 5 },
		{ 0, 2, 3 },
		{ 0, 1, 3, 5 },
		{ 1, 2, 4 },
		{ 3 },
		{ 0, 2 }
	};
	return adj_list_graph{ adjacency };
}

adj_matrix_graph prepare_adj_mat() {
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

template <class MetricMap>
void print(const path& p, const MetricMap& mm) {
    for (const auto& n : p) {
        std::cout << n << ", ";
    }
    std::cout << " -> " << get_weight(p, mm) << std::endl;
}

template <class MetricMap>
void print(const tree& t, const MetricMap& mm) {
	for_each_edge(t, [](node_id u, node_id v) {
        std::cout << u << " - " << v << " -> ";
	});
    std::cout << std::endl;
}

void single_weight_test() {

    std::cout << "Single weight test." << std::endl;

	auto m = prepare_weight();
	auto g1 = prepare_adj_lst();
	auto g2 = prepare_adj_mat();

    g1.set2(5, 4);
    g2.set2(0, 2);

	path pd1 = dijkstra(g1, m, 0, 4);
    print(pd1, m);

	path pd2 = dijkstra(g2, m, 0, 4);
    print(pd2, m);

	path pbf1 = bellman_ford(g1, m, 0, 4);
    print(pbf1, m);

	path pbf2 = bellman_ford(g2, m, 0, 4);
    print(pbf2, m);
    
	bool result = all_equal(pd1, pd2, pbf1, pbf2);
    std::cout << "All paths are " << (result ? "equal" : "not equal") << std::endl;
}

void multi_weight_test() {

    std::cout << "Multi weight test." << std::endl;

	auto m = prepare_weight();

	auto g1 = prepare_adj_lst();
    g1.set2(5, 4);

	auto g2 = prepare_adj_mat();
    g2.set2(0, 2);

	auto ms = prepare_multi_weight();
	path pdm = dijkstra(g1, ms, 0, 4);
    print(pdm, m);

	tree t = prim(g2, m, 0);

    std::cout << std::endl;
}

struct std_line : std::string {
    friend std::istream& operator>>(std::istream& in, std_line& line) {
        return std::getline(in, line);
    }
};

void file_iteration() {
    std::ifstream in { "graph" };

    std::istream_iterator<std_line> first(in);
    std::istream_iterator<std_line> last;

    std::ostream_iterator<std::string> out(std::cout, "\n");

    std::copy(first, last, out);
}

int main() {
    single_weight_test();
    multi_weight_test();
	return 0;
}

