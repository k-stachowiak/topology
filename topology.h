#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <map>
#include <cmath>
#include <deque>
#include <cassert>
#include <iterator>
#include <algorithm>

#include "config.h"

#if 0
concept Topology : Regular {
    static NaturalZero nodes_count(self);
    static ForwardIterator<node> out_begin(self, node);
    static ForwardIterator<node> out_end(self, node);
    static ForwardIterator<edge> edge_begin(self);
    static ForwardIterator<edge> edge_end(self);
}
#endif

#include "topology_graph.h"
#include "topology_path.h"
#include "topology_tree.h"

#endif
