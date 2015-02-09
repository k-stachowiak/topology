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
concept Weight : Regular, TotallyOrdered {
    Weight operator+(Weight, Weight);
};
// NOTE: All numeric built-in types are Weight.
#endif

/// Array of weights.
///
/// @tparam T Type of the Weight concept
/// @tparam M Natural number indicating the count of the aggregated weights
/// @tparam Cmp Comparison functor defining the total ordering of this type.
///
template <typename T, int M, typename Cmp = std::less<T>>
struct multi_weight {

    // Helper typedefs.
    typedef T value_type;
    typedef Cmp value_compare;
    enum { weight_count = M };

    // Implementation of the weight array.
	std::array<T, M> m_impl;

    // Some more typedefs depending on the implementation member.
	typedef typename decltype(m_impl)::iterator iterator;
	typedef typename decltype(m_impl)::const_iterator const_iterator;

    // Enable initialization from explicit input.
	multi_weight(std::initializer_list<T> l) {
		std::copy(begin(l), end(l), begin(m_impl));
        assert(l.size() == M);
    }

    // Enable conversion from a container that provides begin and end iterators.
    template <class Container>
	multi_weight(const Container& values) {
		std::copy(begin(values), end(values), begin(m_impl));
	}

	// Semiregular:
	multi_weight() = default;
	multi_weight(const multi_weight& x) = default;
	multi_weight(multi_weight&& x) : m_impl(x.m_impl) {}
	multi_weight& operator=(const multi_weight& x) = default;
	multi_weight& operator=(multi_weight&& x) {
		m_impl = std::move(x.m_impl);
		return *this;
	}

	// Regular:
	friend bool operator==(const multi_weight& x, const multi_weight& y) { return x == y; }
	friend bool operator!=(const multi_weight& x, const multi_weight& y) { return !(x == y); }

	// Totally ordered:
	friend bool operator<(const multi_weight& x, const multi_weight& y) {
		static Cmp cmp;
		return cmp(x.m_impl, y.m_impl);
	}
	friend bool operator>(const multi_weight& x, const multi_weight& y) { return y < x; }
	friend bool operator<=(const multi_weight& x, const multi_weight& y) { return !(y > x); }
	friend bool operator>=(const multi_weight& x, const multi_weight& y) { return !(y < x); }

	// Weight concept operations:
	friend multi_weight operator+(multi_weight x, const multi_weight& y) {
		for (size_t i = 0; i < x.m_impl.size(); ++i) {
			x.m_impl[i] += y.m_impl[i];
		}
		return x;
	}
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

/// Weight limits traits for the type *multi_weight*.
/// The template arguments are used to instantiate a proper multi_weight type.
///
template <class T, int M, class Cmp>
struct weight_limits<multi_weight<T, M, Cmp>> {
	static multi_weight<T, M, Cmp> inf() {
		multi_weight<T, M, Cmp> result;
		result.m_impl.fill(std::numeric_limits<T>::infinity());
		return result;
	}
	static multi_weight<T, M, Cmp> zero() {
		multi_weight<T, M, Cmp> result;
		result.m_impl.fill(0);
		return result;
	}
	static T one() {
		multi_weight<T, M, Cmp> result;
		result.m_impl.fill(1);
		return result;
	}
};

#endif
