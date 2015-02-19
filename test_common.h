#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <deque>
#include <iostream>

#include "config.h"

template <typename T>
bool is_regular(const T& a, const T& b)
{
    assert (a != b);

    T x;
    x = a;
    T y = b;
    if (x == y) return false;

    x = b;
    if (x != y) return false;

    y = b;
    if (x != y) return false;

    x = a;
    y = x;
    if (x != y) return false;

    return true;
}

template <typename T>
bool is_totally_ordered(const T& a, const T& b)
{
    const T& a_prim = a;

    assert(a < b);

    return 
        a == a_prim &&
        a <= a_prim &&
        a >= a_prim &&
        a_prim <= a &&
        a_prim >= a &&

        a != b &&
        a < b &&
        a <= b &&
        b > a &&
        b >= a;
}

template <typename T>
bool is_additive_monoid(const T& zero, const T& a, const T& b, const T& c)
{
    assert (a != zero && b != zero && c != zero && a != b && b != c);

    return
        zero + a == a &&
        b + zero == b &&
        (a + b) + c == a + (b + c);
}

template <class T>
void print(const T& x)
{
    std::cout << x << " ";
}

template <class T>
void print(const std::deque<T>& d)
{
    std::cout << "deq(" << d.size() << ") : ";
    for (const auto& x : d) {
        print(x);
    }
    std::cout << std::endl;
}

template <class Func>
void for_each_example_edge(Func f)
{
    f(edge { 0, 1 });
    f(edge { 0, 2 });
    f(edge { 0, 5 });
    f(edge { 1, 2 });
    f(edge { 1, 3 });
    f(edge { 2, 3 });
    f(edge { 2, 5 });
    f(edge { 3, 4 });
    f(edge { 4, 5 });
}

template <class Func>
void for_each_example_metric_dbl(Func f)
{
    f(edge { 0, 1 }, 7.0);
    f(edge { 0, 2 }, 9.0);
    f(edge { 0, 5 }, 14.0);
    f(edge { 1, 2 }, 10.0);
    f(edge { 1, 3 }, 15.0);
    f(edge { 2, 3 }, 11.0);
    f(edge { 2, 5 }, 2.0);
    f(edge { 3, 4 }, 6.0);
    f(edge { 4, 5 }, 9.0);
}

void test_weight();
void test_metric();
void test_topology();
void test_algorithm();

#endif
