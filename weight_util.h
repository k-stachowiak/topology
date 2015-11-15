#ifndef WEIGHT_UTIL_H
#define WEIGHT_UTIL_H

#include "weight.h"

#if 0

/// A functor aggregating a MultiWeight into a single weight
concept MultiWeightAggr<MultiWeight> {
    MultiWeight::weight_type operator()(const MultiWeight &);
}

/// A functor comparing two weights
concept WeightCmp<Weight> {
    bool operator()(const Weight&, const Weight&);
}

/// A functor comparing two multi-weights
concept MultiWeightCmp<MultiWeight> {
    bool operator()(const MultiWeight&, const MultiWeight&);
}

#endif

///////////////////////////////////////////////////////////////////////////////
// WeightAggr
///////////////////////////////////////////////////////////////////////////////

template <MultiWeight MW>
struct weight_aggr_lincmb {
    std::vector<double> m_factors;
    using W = typename MW::weight_type;
    W operator()(const MW &mw) const
    {
        using std::begin;
        using std::end;
        assert(MW::weight_count == m_factors.size());
        return std::inner_product(
            begin(m_factors), end(m_factors),
            begin(mw),
            weight_traits<W>::zero());
    }
};

template <MultiWeight MW>
struct weight_aggr_lagrange {
    std::vector<double> m_factors;
    using W = typename MW::weight_type;
    W operator()(const MW &mw) const
    {
        assert(MW::weight_count == m_factors.size() + 1);
        return std::inner_product(
            begin(m_factors), end(m_factors),
            begin(mw) + 1,
            mw[0]);
    }
};

///////////////////////////////////////////////////////////////////////////////
// WeightCmp
///////////////////////////////////////////////////////////////////////////////

/// Weight compare implemented in terms of less than operator.
template <Weight W>
struct weight_cmp_less {
    bool operator()(const W& x, const W& y) const
    {
        return x < y;
    }
};

///////////////////////////////////////////////////////////////////////////////
// MultiWeightCmp
///////////////////////////////////////////////////////////////////////////////

/// Weight compare applicable to multiweights taking into account only a single
/// weight in the sequence.
template <MultiWeight MW, int I>
struct weight_cmp_index {
    bool operator()(const MW& x, const MW& y) const
    {
        return x[I] < y[I];
    }
};

/// Weight compare that compares the so called cost component which is the
/// first value of the multimetric, therefore it can be implemented in
/// terms of the index comparator for multimetrics.
template <MultiWeight MW>
using weight_cmp_cost = weight_cmp_index<MW, 0>;

/// Weight compare that uses an aggregator to compare multiweights as if they
/// were weights.
template <MultiWeight MW, typename Aggr>
struct weight_cmp_aggr {
    Aggr m_aggr;
    bool operator()(const MW& x, const MW& y) const
    {
        return m_aggr(x) < m_aggr(y);
    }
};

#endif
