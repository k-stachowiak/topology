#ifndef METRIC_ADAPTERS_H
#define METRIC_ADAPTERS_H

template <typename Metric>
// Metric is based on the mutli_weight,
class dynamic_index_metric_adapter {

    Metric m_impl;
    int m_index;

public:
    typedef typename std::remove_const<
        typename std::remove_reference<
            typename Metric::weight_type::value_type
        >::type
    >::type weight_type;

    // Semiregular:
    dynamic_index_metric_adapter() = default;
    dynamic_index_metric_adapter(const dynamic_index_metric_adapter&) = default;
    dynamic_index_metric_adapter& operator=(const dynamic_index_metric_adapter&) = default;

    dynamic_index_metric_adapter(dynamic_index_metric_adapter&& x) :
        m_impl { std::move(x.m_impl) },
        m_index { x.m_index }
    {}

    dynamic_index_metric_adapter& operator=(dynamic_index_metric_adapter&& x) {
        m_impl = std::move(x.m_impl);
        m_index = x.m_index;
        return *this;
    }

    // Wrapping constructor:
    dynamic_index_metric_adapter(const Metric& impl, int index) :
        m_impl { impl },
        m_index { index }
    {}

    // Regular:
	friend bool operator==(
            const dynamic_index_metric_adapter& x,
            const dynamic_index_metric_adapter& y) {
        return x.m_impl == y.m_impl; // TODO: Compare only the i-th submetrics?
	}

	friend bool operator!=(
            const dynamic_index_metric_adapter& x,
            const dynamic_index_metric_adapter& y) {
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

template <typename Metric, int I>
// Metric is based on the mutli_weight,
// I is an index within the number of weights in the multi_weight.
class static_index_metric_adapter {

    Metric m_impl;

public:
    typedef typename std::remove_const<
        typename std::remove_reference<
            typename Metric::weight_type::value_type
        >::type
    >::type weight_type;

    // Semiregular:
    static_index_metric_adapter() = default;
    static_index_metric_adapter(const static_index_metric_adapter&) = default;
    static_index_metric_adapter(static_index_metric_adapter&& x) : m_impl { std::move(x.m_impl) } {}
    static_index_metric_adapter& operator=(const static_index_metric_adapter&) = default;
    static_index_metric_adapter& operator=(static_index_metric_adapter&& x) {
        m_impl = std::move(x.impl);
        return *this;
    }

    // Wrapping constructor:
    static_index_metric_adapter(const Metric& impl) : m_impl { impl } {}
    static_index_metric_adapter(Metric&& impl) : m_impl { std::move(impl) } {}

    // Regular:
	friend bool operator==(const static_index_metric_adapter& x, const static_index_metric_adapter& y) {
        return x.m_impl == y.m_impl; // TODO: Compare only the i-th submetrics?
	}
	friend bool operator!=(const static_index_metric_adapter& x, const static_index_metric_adapter& y) {
		return !(x == y);
	}

	// Operations:
    const weight_type operator()(node_id from, node_id to) const {
		return m_impl(from, to).m_impl.at(I);
	}
    weight_type& operator()(node_id from, node_id to) {
		return m_impl(from, to).m_impl[I];
	}
};

#endif
