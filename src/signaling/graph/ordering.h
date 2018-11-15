//
// Created by smaptas on 09/14/18.
//

#ifndef CAPS_ORDERING_H
#define CAPS_ORDERING_H

#include <functional>
#include <unordered_set>

#include "labeled_graph/graph.h"
#include "node/node.h"
#include "../common/iterable.h"

namespace {
  using NodeHandle = const Node*;
  using Order = std::vector<NodeHandle>;
  using Set = std::unordered_set<NodeHandle>;
}

ReverseIterable<Order> topological_order(const LabeledGraph& graph);

Order reverse_topological_order(const LabeledGraph& graph);

std::unordered_map<NodeHandle, int> node_to_order_map(const Order& order);

std::map<int, NodeHandle> order_to_node_map(const Order& order);

#endif //CAPS_ORDERING_H
