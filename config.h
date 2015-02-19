#ifndef CONFIG_H
#define CONFIG_H

#include <utility>
#include <algorithm>

using node = int;
using edge = std::pair<node, node>;

inline edge normalize(const edge& e) { return std::minmax(e.first, e.second); }
inline edge reverse(const edge& e) { return { e.second, e.first }; }

#endif
