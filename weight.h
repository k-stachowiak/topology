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

// concept Weight w
// - is regular
// - is totally ordered
//
// operations:
// - (friend) + : w -> w -> w
//
// NOTE: ALL NUMERIC BUILT-IN TYPES SATISFY THESE REQUIREMENTS.

/// Weight comcept implementation to aggregate many weights into single
/// objects.
template <typename T, int M, typename Cmp = std::less<T>>
// T is a Weight,
// M is a natural number - the count of the weights,
// Cmp defines a total ordering of an array of weights.
struct multi_weight {

    typedef T value_type;
    typedef Cmp value_compare;
    enum { weight_count = M };

	// This type wraps a collection of weights (of type T) into
	// an array, and implements the typical arythmetic operations
	// in a member by member fashion.
	//
	// The M argument defines the number of weights stored in the type
	// and guarantees compile time protection against operations on
	// multi-weights of different weight counts.
	//
	// Additionally the Cmp functor type must be provided for
	// implementation of the total ordering, which cannot be easily
	// implemented member by member.

	std::array<T, M> m_impl;

	multi_weight(const std::array<T, M>& values) {
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
	friend bool operator==(const multi_weight& x, const multi_weight& y) {
		return x == y;
	}
	friend bool operator!=(const multi_weight& x, const multi_weight& y) {
		return !(x == y);
	}

	// Totally ordered:
	friend bool operator<(const multi_weight& x, const multi_weight& y) {
		static Cmp cmp;
		return cmp(x.m_impl, y.m_impl);
	}
	friend bool operator>(const multi_weight& x, const multi_weight& y) {
		return y < x;
	}
	friend bool operator<=(const multi_weight& x, const multi_weight& y) {
		return !(y > x);
	}
	friend bool operator>=(const multi_weight& x, const multi_weight& y) {
		return !(y < x);
	}

	// Operations:
	friend multi_weight operator+(multi_weight x, const multi_weight& y) {
		for (size_t i = 0; i < x.m_impl.size(); ++i) {
			x.m_impl[i] += y.m_impl[i];
		}
		return x;
	}
};

// concept WeightLimits ml<T>
//
// operations:
// - (static) inf : () -> T::infinity
// - (static) zero : () -> T::zero
// - (static) one : () -> T::one

template <class T>
// T is a built-in numeric type accepted by std::numeric_limits.
struct weight_limits {
	static T inf() { return std::numeric_limits<T>::infinity(); }
	static T zero() { return 0; }
	static T one() { return 1; }
};

template <class T, int M, class Cmp>
// Requirements for T, M and Cmp the same as in the multi_weight type.
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
