#ifndef WEIGHT_AGGREGATE_H
#define WEIGHT_AGGREGATE_H

// Weight aggregation:
//
// This idea consists in being able to map topological structures to 
// metric values:
//
// path -> metric_map -> metric
// tree -> metric_map -> metric
// ...

// Basic metric aggregation.
template <class Metric>
typename Metric::weight_type get_weight(const path& p, const Metric& m) {

    auto first = p.begin();
    auto last = p.end();

    typename Metric::weight_type result = weight_limits<
        typename Metric::weight_type>::zero();

    if (first == last)  {
        return result;
    }

    while (first + 1 != last) {
        auto next = first + 1;
        result = result + m(*first, *next);
        first = next;
    }

    return result;
}

#endif
