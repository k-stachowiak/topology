#include <cassert>

#include "test_common.h"
#include "metric.h"
#include "weight.h"

namespace {

    void hop_test()
    {
        hop_metric<double> x;
        hop_metric<double> y;
        assert(x == y);

        double x_metric = x(edge { 1, 2 });
        assert(x_metric == 1);

        double y_metric = y(edge { 3, 4 });
        assert(x_metric == y_metric);
    }

    void map_test()
    {
        map_metric<int, false> x, y;
        x(edge(1, 2)) = 1;
        x(edge(2, 1)) = 2;
        y(edge(1, 2)) = x(edge { 1, 2 } );
        assert(x != y);
        y(edge(2, 1)) = x(edge { 2, 1 } );
        assert(x == y);

        map_metric<int, true> s, t;
        s(edge(1, 2)) = 1;
        s(edge(2, 1)) = 2;
        t(edge(1, 2)) = 2;
        assert(s == t);
    }

}

void test_metric()
{
    hop_test();
    map_test();
}
