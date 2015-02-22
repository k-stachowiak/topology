#include <cassert>

#include "test_common.h"
#include "weight.h"

namespace {

    template <class Weight>
    bool is_weight()
    {
        Weight zero = weight_limits<Weight>::zero();
        Weight a = weight_limits<Weight>::one();
        Weight b = weight_limits<Weight>::one() + weight_limits<Weight>::one();
        Weight c = weight_limits<Weight>::one() + weight_limits<Weight>::one() + weight_limits<Weight>::one();
        return is_regular(a, b) && is_additive_monoid(zero, a, b, c);
    }

}

void test_weight()
{
    assert((is_weight<double>()));
    assert((is_weight<array_weight<double, 2>>()));
    assert((is_weight<array_weight<int, 1>>()));
}

