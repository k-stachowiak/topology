#ifndef METRIC_H
#define METRIC_H

#include <cassert>

#include <map>
#include <array>
#include <deque>
#include <utility>
#include <iterator>
#include <algorithm>

#include "config.h"

// concept Metric m
// - is regular
// - is totally ordered
//
// operations:
// - (friend) + : m -> m -> m
//
// NOTE: ALL NUMERIC BUILT-IN TYPES SATISFY THESE REQUIREMENTS.

template <typename T, int M, typename Cmp = std::less<T>>
// T is a Metric,
// M is a natural number - the count of the metrics,
// Cmp defines a total ordering of an array of Metrics.
struct multi_metric {

    typedef T metric_type;
    typedef Cmp metric_compare;
    enum { metric_count = M };

	// This type wraps a collection of metrics (of type T) into
	// an array, and implements the typical arythmetic operations
	// in a member by member fashion.
	//
	// The M argument defines the number of metrics stored in the type
	// and guarantees compile time protection against operations on
	// multi-metrics of different metric counts.
	//
	// Additionally the Cmp functor type must be provided for
	// implementation of the total ordering, which cannot be easily
	// implemented member by member.

	std::array<T, M> m_impl;

	multi_metric(const std::array<T, M>& values) {
		std::copy(begin(values), end(values), begin(m_impl));
	}

	// Semiregular:
	multi_metric() = default;
	~multi_metric() = default;
	multi_metric(const multi_metric& x) = default;
	multi_metric(multi_metric&& x) : m_impl(x.m_impl) {}
	multi_metric& operator=(const multi_metric& x) = default;
	multi_metric& operator=(multi_metric&& x) {
		m_impl = std::move(x.m_impl);
		return *this;
	}

	// Regular:
	friend bool operator==(const multi_metric& x, const multi_metric& y) {
		return x == y;
	}
	friend bool operator!=(const multi_metric& x, const multi_metric& y) {
		return !(x == y);
	}

	// Totally ordered:
	friend bool operator<(const multi_metric& x, const multi_metric& y) {
		static Cmp cmp;
		return cmp(x.m_impl, y.m_impl);
	}
	friend bool operator>(const multi_metric& x, const multi_metric& y) {
		return y < x;
	}
	friend bool operator<=(const multi_metric& x, const multi_metric& y) {
		return !(y > x);
	}
	friend bool operator>=(const multi_metric& x, const multi_metric& y) {
		return !(y < x);
	}

	// Operations:
	friend multi_metric operator+(multi_metric x, const multi_metric& y) {
		for (size_t i = 0; i < x.m_impl.size(); ++i) {
			x.m_impl[i] += y.m_impl[i];
		}
		return x;
	}
};

// concept MetricLimits ml<T>
//
// operations:
// - (static) inf : () -> T::infinity
// - (static) zero : () -> T::zero
// - (static) one : () -> T::one

template <class T>
// T is a built-in numeric type accepted by std::numeric_limits.
struct metric_limits {
	static T inf() { return std::numeric_limits<T>::infinity(); }
	static T zero() { return 0; }
	static T one() { return 1; }
};

template <class T, int M, class Cmp>
// Requirements for T, M and Cmp the same as in the multi_metric type.
struct metric_limits<multi_metric<T, M, Cmp>> {
	static multi_metric<T, M, Cmp> inf() {
		multi_metric<T, M, Cmp> result;
		result.m_impl.fill(std::numeric_limits<T>::infinity());
		return result;
	}
	static multi_metric<T, M, Cmp> zero() {
		multi_metric<T, M, Cmp> result;
		result.m_impl.fill(0);
		return result;
	}
	static T one() {
		multi_metric<T, M, Cmp> result;
		result.m_impl.fill(1);
		return result;
	}
};

#endif
