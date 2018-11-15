//
// Created by smaptas on 09/14/18.
//

#include <unordered_set>

#include "ordering.h"
#include "../common/contains.h"

ReverseIterable<Order> topological_order(const LabeledGraph& graph)
{
  return {reverse_topological_order(graph)};
}

Order reverse_topological_order(const LabeledGraph& graph)
{
  // Assumes Order is vector
  Order order;
  order.reserve(graph.get_num_nodes());

  std::unordered_set<NodeHandle> ordered;
  std::stack<std::pair<NodeHandle, bool>> stack;

  stack.emplace(graph.get_root(), false);

  while (!stack.empty()) {
    auto [node, all_children_ordered] = stack.top();
    stack.pop();
    if (ordered.find(node) != ordered.end()) {
      continue;
    }
    if (all_children_ordered) {
      order.push_back(node);
      ordered.insert(node);
    } else {
      stack.emplace(node, true);
      for (const auto& [label, child]: node->get_out_edges()) {
        if (ordered.find(child) == ordered.end()) {
          stack.emplace(child, false);
        }
      }
    }
  }

  return order;
}

std::unordered_map<NodeHandle, int> node_to_order_map(const Order& order)
{
  std::unordered_map<NodeHandle, int> map;
  for (size_t i = 0; i < order.size(); ++i) {
    map.emplace(order[i], static_cast<int>(i));
  }
  return map;
}

std::map<int, NodeHandle> order_to_node_map(const Order& order)
{
  std::map<int, NodeHandle> map;
  for (size_t i = 0; i < order.size(); ++i) {
    map.emplace(i, order[i]);
  }
  return map;
}

