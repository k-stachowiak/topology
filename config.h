#ifndef CONFIG_H
#define CONFIG_H

#include <utility>
#include <algorithm>

using node = int;
using edge = std::pair<node, node>;

inline edge normalize(const edge& e) { return std::minmax(e.first, e.second); }
inline edge reverse(const edge& e) { return { e.second, e.first }; }

template <class T>
constexpr bool is_regular()
{
    return
        std::is_default_constructible<T>::value &&
        std::is_copy_constructible<T>::value &&
        std::is_copy_assignable<T>::value &&
        std::is_move_constructible<T>::value &&
        std::is_move_assignable<T>::value &&
        std::is_destructible<T>::value;
}

#endif
