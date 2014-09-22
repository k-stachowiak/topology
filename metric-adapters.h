#ifndef METRIC_ADAPTERS_H
#define METRIC_ADAPTERS_H

namespace detail {

    template <typename Metric, typename Func, typename CFunc>
    // Metric is based on the mutli_weight,
    class aggregating_metric_adapter {

        Metric m_impl;
        Func m_func;
        CFunc m_cfunc;

    public:
        typedef typename std::remove_const<
            typename std::remove_reference<
                typename Metric::weight_type::value_type
            >::type
        >::type weight_type;

        // Semiregular:
        aggregating_metric_adapter() = default;
        aggregating_metric_adapter(const aggregating_metric_adapter&) = default;
        aggregating_metric_adapter& operator=(const aggregating_metric_adapter&) = default;

        aggregating_metric_adapter(aggregating_metric_adapter&& x) :
            m_impl { std::move(x.m_impl) },
            m_func { x.m_func }
            m_cfunc { x.m_cfunc }
        {}

        aggregating_metric_adapter& operator=(aggregating_metric_adapter&& x) {
            m_impl = std::move(x.m_impl);
            m_cfunc = x.m_cfunc;
            return *this;
        }

        // Wrapping constructor:
        aggregating_metric_adapter(const Metric& impl, Func func, CFunc cfunc) :
            m_impl { impl },
            m_func { func }
            m_cfunc { cfunc }
        {}

        // Regular:
        friend bool operator==(
                const aggregating_metric_adapter& x,
                const aggregating_metric_adapter& y) {
            return x.m_impl == y.m_impl; // TODO: Compare only the i-th submetrics?
        }

        friend bool operator!=(
                const aggregating_metric_adapter& x,
                const aggregating_metric_adapter& y) {
            return !(x == y);
        }

        // Operations:
        const weight_type operator()(node_id from, node_id to) const {
            const auto& seq = m_impl(from, to).m_impl;
            return m_cfunc(begin(seq), end(seq));
        }
        weight_type& operator()(node_id from, node_id to) {
            const auto& seq = m_impl(from, to).m_impl;
            return m_func(begin(seq), end(seq));
        }
    };

}

template <typename Metric>
class index_metric_adapter {

    auto m_func = [](){};
    auto m_cfunc = [](){};

    aggregating_metric_adapter<Metric, decltype(m_func), decltype(m_cfunc)> m_impl;

    typedef m_impl::weight_type weight_type;

    // Semiregular:
    index_metric_adapter() = default;
    index_metric_adapter(const index_metric_adapter&) = default;
    index_metric_adapter& operator=(const index_metric_adapter&) = default;

    index_metric_adapter(index_metric_adapter&& x) :
        m_impl { std::move(x.m_impl) },
        m_index { x.m_index }
    {}

    index_metric_adapter& operator=(index_metric_adapter&& x) {
        m_impl = std::move(x.m_impl);
        m_index = x.m_index;
        return *this;
    }
}

template <typename Metric>
// Metric is based on the mutli_weight,
class index_metric_adapter2 {

    Metric m_impl;
    int m_index;

public:
    typedef typename std::remove_const<
        typename std::remove_reference<
            typename Metric::weight_type::value_type
        >::type
    >::type weight_type;

    // Semiregular:
    index_metric_adapter() = default;
    index_metric_adapter(const index_metric_adapter&) = default;
    index_metric_adapter& operator=(const index_metric_adapter&) = default;

    index_metric_adapter(index_metric_adapter&& x) :
        m_impl { std::move(x.m_impl) },
        m_index { x.m_index }
    {}

    index_metric_adapter& operator=(index_metric_adapter&& x) {
        m_impl = std::move(x.m_impl);
        m_index = x.m_index;
        return *this;
    }

    // Wrapping constructor:
    index_metric_adapter(const Metric& impl, int index) :
        m_impl { impl },
        m_index { index }
    {}

    // Regular:
	friend bool operator==(const index_metric_adapter& x, const index_metric_adapter& y) {
        return x.m_impl == y.m_impl; // TODO: Compare only the i-th submetrics?
	}

	friend bool operator!=(const index_metric_adapter& x, const index_metric_adapter& y) {
		return !(x == y);
	}

	// Operations:
    const weight_type operator()(node_id from, node_id to) const {
		return m_impl(from, to).m_impl.at(m_index);
	}
    weight_type& operator()(node_id from, node_id to) {
		return m_impl(from, to).m_impl[m_index];
	}
};

#endif
