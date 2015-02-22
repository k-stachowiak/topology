#ifndef WEIGHT_H
#define WEIGHT_H

#include <cassert>

#include <map>
#include <array>
#include <deque>
#include <utility>
#include <iterator>
#include <algorithm>

#include "config.h"

#if 0

concept Weight : Regular {
    Weight operator+(Weight, Weight);
};

concept MultiWeight<Weight> : Weight {
    Weight operator[](int);
    ForwardIterator<Weight> begin();
    ForwardIterator<Weight> end();
};

#endif

/// Array of weights.
/// The difference between the array_weight and std::array lies in the definition
/// of the +operator.
///
/// @tparam T Underlying Weight
/// @tparam M Natural+ number indicating the count of the aggregated weights
///
template <typename Weight, int M>
struct array_weight {

    // Helper typedefs.
    typedef Weight weight_type;
    enum { weight_count = M };

    // Implementation of the weight array.
	std::array<Weight, M> m_impl;

    // Some more typedefs depending on the implementation member.
	typedef typename decltype(m_impl)::iterator iterator;
	typedef typename decltype(m_impl)::const_iterator const_iterator;

    // Enable initialization from explicit input.
	array_weight(std::initializer_list<Weight> l) {
		std::copy(l.begin(), l.end(), m_impl.begin());
        assert(l.size() == M);
    }

    // Enable conversion from a container that provides begin and end iterators.
    template <class Container>
	array_weight(const Container& values) {
		std::copy(begin(values), end(values), begin(m_impl));
	}

	// Semiregular:
	array_weight() = default;
	array_weight(const array_weight& x) = default;
	array_weight(array_weight&& x) : m_impl(x.m_impl) {}
	array_weight& operator=(const array_weight& x) = default;
	array_weight& operator=(array_weight&& x) {
		m_impl = std::move(x.m_impl);
		return *this;
	}

	// Regular:
	friend bool operator==(const array_weight& x, const array_weight& y) { return x.m_impl == y.m_impl; }
	friend bool operator!=(const array_weight& x, const array_weight& y) { return !(x == y); }

	// Weight operations:
	friend array_weight operator+(array_weight x, const array_weight& y) {
		for (typename decltype(x.m_impl)::size_type i = 0; i < x.m_impl.size(); ++i) {
			x.m_impl[i] += y.m_impl[i];
		}
		return x;
	}

    // MultiWeight operations:
    const Weight operator[](int index) const { return m_impl[index]; }
    iterator begin() { return m_impl.begin(); }
    const_iterator begin() const { return m_impl.begin(); }
    iterator end() { return m_impl.end(); }
    const_iterator end() const { return m_impl.end(); }
};

#if 0
concept WeightLimits<T> {
    static T inf();
    static T zero();
    static T one();
};
#endif

/// Trait class enabling providing of basic numeric values.
/// It is specialized later on for custom types.
///
/// @tparam The built-in numeric type.
///
template <class T>
struct weight_limits {
	static T inf() { return std::numeric_limits<T>::infinity(); }
	static T zero() { return 0; }
	static T one() { return 1; }
};

/// Weight limits traits for the type *array_weight*.
/// The template arguments are used to instantiate a proper array_weight type.
///
template <class T, int M>
struct weight_limits<array_weight<T, M>> {

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
