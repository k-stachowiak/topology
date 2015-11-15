#ifndef WEIGHT_H
#define WEIGHT_H

#include <cassert>

#include <map>
#include <array>
#include <deque>
#include <utility>
#include <iterator>
#include <algorithm>
#include <type_traits>

#include "config.h"

#if 0

/// Essentially weight forms an additive semigroup over the set of the real
/// numbers. Note that the order is not required for the weight concept as
/// the less then is to be provided ad-hoc by the client.
concept Weight : Regular {
    static self operator+(self, self);
};

/// The multi-weight may be used just as the weight, but it additionally
/// provides tools for the introspection of its components.
concept MultiWeight<Weight> : Weight {
    typename Weight weight_type;
    static const int weight_count;
    Weight operator[](int);
    ForwardIterator<Weight> begin();
    ForwardIterator<Weight> end();
};

/// Provides basic values for the algorithms to operate on the custom weights.
concept WeightTraits<T> {
    static T inf();
    static T zero();
    static T one();
};

#endif

///////////////////////////////////////////////////////////////////////////////
// Weight
///////////////////////////////////////////////////////////////////////////////

#define Weight typename

// Weight concept is fulfilled by built-in numeric types and therefore there
// has not yet been a necessity to implement a user defined type for this.

///////////////////////////////////////////////////////////////////////////////
// MultiWeight
///////////////////////////////////////////////////////////////////////////////

#define MultiWeight typename

/// Array of weights.
/// The difference between the array_weight and std::array lies in the definition
/// of the +operator.
///
/// @tparam W Underlying Weight
/// @tparam M Natural+ number indicating the count of the aggregated weights
///
template <Weight W, int M>
struct array_weight {

    typedef W weight_type;
    static const int weight_count = M;

    std::array<W, M> m_impl;
    typedef typename decltype(m_impl)::iterator iterator;
    typedef typename decltype(m_impl)::const_iterator const_iterator;

    // Custom constructors:
    array_weight(std::initializer_list<W> l)
    {
        assert(l.size() == M);
        std::copy(l.begin(), l.end(), m_impl.begin());
    }

    template <class Container>
    array_weight(const Container& values)
    {
        std::copy(begin(values), end(values), begin(m_impl));
    }

    // Semiregular:
    array_weight() = default;
    array_weight(const array_weight& x) = default;
    array_weight(array_weight&& x) : m_impl(x.m_impl) {}
    array_weight& operator=(const array_weight& x) noexcept = default;
    array_weight& operator=(array_weight&& x) noexcept = default;

    // Regular:
    friend bool operator==(const array_weight& x, const array_weight& y) { return x.m_impl == y.m_impl; }
    friend bool operator!=(const array_weight& x, const array_weight& y) { return !(x == y); }

    // Weight operations:
    friend array_weight operator+(array_weight x, const array_weight& y)
    {
        for (typename decltype(x.m_impl)::size_type i = 0; i < x.m_impl.size(); ++i) {
            x.m_impl[i] += y.m_impl[i];
        }
        return x;
    }

    // MultiWeight operations:
    const W& operator[](int index) const { return m_impl[index]; }
    iterator begin() { return m_impl.begin(); }
    const_iterator begin() const { return m_impl.begin(); }
    iterator end() { return m_impl.end(); }
    const_iterator end() const { return m_impl.end(); }
};

///////////////////////////////////////////////////////////////////////////////
// Weight traits
///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct weight_traits {
    static T inf() { return std::numeric_limits<T>::max(); }
    static T zero() { return 0; }
    static T one() { return 1; }
};

/// Weight limits traits for the type *array_weight*.
/// The template arguments are used to instantiate a proper array_weight type.
///
template <class T, int M>
struct weight_traits<array_weight<T, M>> {

    static array_weight<T, M> inf()
    {
        array_weight<T, M> result;
        result.m_impl.fill(std::numeric_limits<T>::infinity());
        return result;
    }

    static array_weight<T, M> zero()
    {
        array_weight<T, M> result;
        result.m_impl.fill(0);
        return result;
    }

    static array_weight<T, M> one()
    {
        array_weight<T, M> result;
        result.m_impl.fill(1);
        return result;
    }

};

#endif
