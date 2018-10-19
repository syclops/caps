//
// Created by smaptas on 09/14/18.
//

#include <unordered_set>

#include "ordering.h"
#include "../common/contains.h"

ReverseIterable<OrderType> topological_order(const LabeledGraph& graph)
{
  return {reverse_topological_order(graph)};
}

OrderType reverse_topological_order(const LabeledGraph& graph)
{
  // Assumes OrderType is vector
  OrderType order;
  order.reserve(static_cast<unsigned long>(graph.get_num_nodes()));

  std::unordered_set<NodePtrType> visited;
  std::stack<std::pair<NodePtrType, bool>> stack;

  stack.push(std::make_pair(graph.get_root(), false));

  while (!stack.empty()) {
    auto pair = stack.top();
    stack.pop();
    if (contains(visited, pair.first)) {
      continue;
    }
    if (pair.second) {
      visited.insert(pair.first);
      order.push_back(pair.first);
    } else {
      stack.push(std::make_pair(pair.first, true));
      for (const auto& out_edge: pair.first->get_out_edges()) {
        if (!contains(visited, out_edge.second)) {
          stack.push(std::make_pair(out_edge.second, false));
        }
      }
    }
  }

  return order;
}

std::unordered_map<NodePtrType, int> node_to_order_map(const OrderType& order)
{
  std::unordered_map<NodePtrType, int> map;
  for (size_t i = 0; i < order.size(); ++i) {
    map[order[i]] = i;
  }
  return map;
}

std::map<int, NodePtrType> order_to_node_map(const OrderType& order)
{
  std::map<int, NodePtrType> map;
  for (size_t i = 0; i < order.size(); ++i) {
    map[i] = order[i];
  }
  return map;
}

