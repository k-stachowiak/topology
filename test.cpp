#include <cassert>
#include <iostream>

#include "weight.h"
#include "metric.h"

template <class T>
void assert_regular_totally_ordered_plus(T zero, T one)
{
    T x;
    x = zero;

    T y = x;

    assert(x == y);
    assert(x <= y);
    assert(x >= y);
    assert(y <= x);
    assert(y >= x);

    x = x + one;

    assert(y != x);
    assert(y < x);
    assert(y <= x);
    assert(x > y);
    assert(x >= y);

    y = y + one;
    y = y + one;

    assert(x != y);
    assert(x < y);
    assert(x <= y);
    assert(y > x);
    assert(y >= x);

    x = x + one;

    assert(x == y);
    assert(x <= y);
    assert(x >= y);
    assert(y <= x);
    assert(y >= x);
}

void metric_hop_test()
{
    hop_metric<double> x;
    hop_metric<double> y;
    assert(x == y);

    double x_metric = x(edge { 1, 2 });
    assert(x_metric == 1);

    double y_metric = y(edge { 3, 4 });
    assert(x_metric == y_metric);
}

void metric_map_test()
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

void metric_index_adapter_test()
{
    using weight_type = multi_weight<double, 2>;

    edge e1 { 0, 1 };
    edge e2 { 1, 2 };

    map_metric<weight_type, false> m;
    m(e1) = weight_type { 1.0, 2.0 };
    m(e2) = weight_type { 2.0, 1.0 };

    index_metric_adapter<decltype(m)> ma0 { &m, 0 };
    index_metric_adapter<decltype(m)> ma1 { &m, 1 };

    assert(ma0(e1) == ma1(e2));
    assert(ma1(e1) == ma0(e2));
}

void test()
{
    using two_dbl = multi_weight<double, 2>;
    assert_regular_totally_ordered_plus<two_dbl>(
            weight_limits<two_dbl>::zero(),
            weight_limits<two_dbl>::one());

    using one_int = multi_weight<int, 1>;
    assert_regular_totally_ordered_plus<one_int>(
            weight_limits<one_int>::zero(),
            weight_limits<one_int>::one());

    metric_hop_test();
    metric_map_test();
    metric_index_adapter_test();
}

