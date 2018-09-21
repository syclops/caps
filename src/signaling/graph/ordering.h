//
// Created by smaptas on 09/14/18.
//

#ifndef CAPS_ORDERING_H
#define CAPS_ORDERING_H

#include <unordered_set>

#include "graph.h"
#include "node.h"
#include "../common/iterable.h"

namespace {
  using NodePtrType = std::shared_ptr<Node>;
  using OrderType = std::vector<NodePtrType>;
  using SetType = std::unordered_set<NodePtrType>;
}

ReverseIterable<OrderType> topological_order(const LabeledGraph& graph);

OrderType reverse_topological_order(const LabeledGraph& graph);

std::unordered_map<NodePtrType, int> node_to_order_map(const OrderType& order);

std::map<int, NodePtrType> order_to_node_map(const OrderType& order);

#endif //CAPS_ORDERING_H
