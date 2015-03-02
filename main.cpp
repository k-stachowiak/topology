#include <iterator>
#include <iostream>
#include <fstream>
#include <vector>

#include "test_common.h"

void test();

// TODO: test/pix prim

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

