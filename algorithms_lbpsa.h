#ifndef ALGORITHMS_LBPSA_H
#define ALGORITHMS_LBPSA_H

namespace detail {

    bool lbpsa_conditions(typename Metric::weight_type current_weight, node dst)
    {
        using Weight = typename Metric::weight_type;
        Weight previous_weight = frasible_costs[dst];

		// Other.
		// ------
		double upperBound = upper_bound;

		double weightedConstraints = 0.0;
		for (int m = 1; m < numMetrics; ++m) {
			weightedConstraints += feasibleFinder.getLambdas().get(m - 1)
					* constraints.get(m - 1);
		}

		// Conditions.
		// -----------
		// Condition 1:
		boolean cnd1 = fwdEdge.getMetrics().get(0)
				+ revEdge.getMetrics().get(0) <= upperBound;

		if (!cnd1) {
			return false;
		}

		// Condition 2:
		boolean cnd2 = true;
		for (int i = 1; i < numMetrics; ++i) {

			double value = fwdEdge.getMetrics().get(i)
					+ revEdge.getMetrics().get(i) - constraints.get(i - 1);

			if (value > 0.0) {
				cnd2 = false;
				break;
			}
		}
		if (!cnd2) {
			return false;
		}

		// Condition 3:
		boolean cnd3 = current_weight + previous_weight <= upperBound
				+ weightedConstraints;

		if (!cnd3) {
			return false;
		}

		return true;
	}

    template <class Graph, class Metric>
    void lbpsa_rec(
            const Graph& g, const Metric& m,
            const std::vector<double>& cstr,
            node src, node dst,
            typename Metric::weight_type &current_weight,
            path &current_path,
            std::vector<path> &feasible)
    {
        using Weight = typename Metric::weight_type;

        node u = current_path.back();

		if (u == src) {
			feasible.push_back(current_path);
			return;
		}

        auto first = out_begin(g, u);
        const auto last = out_end(g, u);
        while (first != last) {

            node v = *first++;

			if (std::find(begin(current_path), end(current_path), v) != end(path)) {
				continue;
			}

			edge e { u, v };

            Weight old_weight = current_weight;
            current_weight = old_weight + m(e);
			if (detail::lbpsa_conditions(e)) {
				currentPath.push_back(v);
				lbpsa_rec(g, m, cstr, src, dst, current_weight, current_path, feasible);
				currentPath.pop_back();
			}
            current_weight = old_weight;
		}
    }

}

template <class Graph, class Metric>
path lbpsa(const Graph& g, const Metric& m, const std::vector<double>& cstr, node src, node dst)
{
    using Weight = typename Metric::weight_type;

    Weight wth = weight_traits<Weight>::zero();
    path res { dst };
    std::vector<path> fsbl;

    detail::lbpsa_rec(g, m, cstr, src, dst, wth, res, fsbl);

    if (fsbl.empty()) {
        return {};

    } else {
        return *std::min_element(begin(fsbl), end(fsbl), [&m](const path& x, const path& y) {
            return accumulate_weight(m, x) < accumulate_weight(m, y);
        });
    }
}

#endif
