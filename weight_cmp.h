#ifndef WEIGHT_CMP_H
#define WEIGHT_CMP_H

#if 0

concept WeightCmp<Weight> : Functor {
    bool operator()(const Weight&, const Weight&);
}

#endif

template <typename Weight>
struct weight_cmp_less {
    bool operator()(const Weight& x, const Weight& y)
    {
        return x < y;
    }
};

template <class MultiWeight, int I>
struct weight_cmp_index {
    bool operator()(const MultiWeight& x, const MultiWeight& y) const
    {
		return x[I] < y[I];
    }
};

template <class MultiWeight>
using weight_cmp_cost = weight_cmp_index<MultiWeight, 0>;

template <class MultiWeight>
class weight_cmp_lin_cmb {

    using Weight = typename MultiWeight::weight_type;
    enum {
        weight_count = MultiWeight::weight_count,
        coef_count = weight_count
    };

    std::array<double, coef_count> m_coefficients;

    template <typename Iter>
    void init_from_iter(Iter begin, Iter end)
    {
        const int distance = std::distance(begin, end);
        assert(distance == coef_count);
        std::copy(begin, end, m_coefficients.begin());
    }

    Weight aggregate(const MultiWeight& x) const
    {
        Weight result = weight_limits<Weight>::zero();
        for (int i = 0; i < coef_count; ++i) {
            result += x[i] * m_coefficients[i];
        }
        return result;
    }

public:
    template <typename Iter>
    weight_cmp_lin_cmb(Iter begin, Iter end)
    {
        init_from_iter(begin, end);
    }

    weight_cmp_lin_cmb(std::initializer_list<double> coefficients)
    {
        init_from_iter(coefficients.begin(), coefficients.end());
    }

    bool operator()(const MultiWeight& x, const MultiWeight& y) const
    {
        return aggregate(x) < aggregate(y);
    }
};

template <class MultiWeight>
class weight_cmp_lagrange {

    using Weight = typename MultiWeight::weight_type;
    enum {
        weight_count = MultiWeight::weight_count,
        coef_count = weight_count - 1,
        constr_count = coef_count
    };

    std::array<double, coef_count> m_coefficients;
    std::array<double, constr_count> m_constraints;

    template <typename CoefIter, typename ConstrIter>
    void init_from_iters(
            CoefIter coef_begin, CoefIter coef_end,
            ConstrIter constr_begin, ConstrIter constr_end)
    {
        const int coef_distance = std::distance(coef_begin, coef_end);
        assert(coef_distance == coef_count);
        std::copy(coef_begin, coef_end, begin(m_coefficients));

        const int constr_distance = std::distance(constr_begin, constr_end);
        assert(constr_distance == constr_count);
        std::copy(constr_begin, constr_end, begin(m_constraints));
    }

    Weight aggregate(const MultiWeight& x) const
    {
        Weight result = x[0];
        for (int i = 1; i < coef_count; ++i) {
            result += (x[i] - m_constraints[i - 1]) * m_coefficients[i - 1];
        }
        return result;
    }

public:
    template <typename CoefIter, typename ConstrIter>
    weight_cmp_lagrange(
            CoefIter coef_begin, CoefIter coef_end,
            ConstrIter constr_begin, ConstrIter constr_end)
    {
        init_from_iters(coef_begin, coef_end, constr_begin, constr_end);
    }

    bool operator()(const MultiWeight& x, const MultiWeight& y) const
    {
        return aggregate(x) < aggregate(y);
    }
};

#endif
