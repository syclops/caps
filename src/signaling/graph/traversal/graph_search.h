//
// Created by smaptas on 26.10.17.
//

#ifndef CAPS_GRAPH_SEARCH_H
#define CAPS_GRAPH_SEARCH_H

#include <memory>
#include <queue>
#include <stack>
#include <unordered_set>

#include "../labeled_graph/graph.h"
#include "../node/node.h"
#include "../visitor/visitor.h"

template <typename Visitor>
void visit_dfs(std::shared_ptr<Visitor> visitor, Node* source)
{
  std::stack<std::pair<std::shared_ptr<Visitor>, Node*>> stack;
  stack.emplace(std::make_pair(visitor, source));
  std::unordered_set<Node*> visited;
  visited.insert(source);
  while (!stack.empty()) {
    auto pair = stack.top();
    stack.pop();
    auto node_copy = pair.first->clone();
    node_copy->process_node(pair.second);
    for (const auto &out_edge_itr: pair.second->get_reverse_out_edges()) {
      if (node_copy->should_proceed(pair.second, out_edge_itr.second,
                                    out_edge_itr.first)) {
        auto label_copy = node_copy->clone();
        label_copy->process_label(out_edge_itr.first);
        stack.emplace(std::make_pair(label_copy, out_edge_itr.second));
      }
    }
  }
}

template <typename Iterator>
void depth_first_search(Visitor<Iterator>& visitor, const Node* start)
{
  // Initialize data structures used in the rest of this algorithm.
  std::stack<const Node*> stack;
  stack.push(start);
  visitor.setup();

  // Perform the standard DFS algorithm using the now non-empty stack.
  while (!stack.empty()) {
    auto node = stack.top();
    stack.pop();
    if (visitor.should_visit_node(node)) {
      visitor.visit_node(node);
    }
    for (auto i = visitor.begin(node); i != visitor.end(node); ++i) {
      const auto& [label, dest] = *i;
      if (visitor.should_visit_edge(node, dest, label)) {
        visitor.visit_edge(node, dest, label);
        stack.emplace(dest);
      }
    }
  }
  visitor.finish();
}

template <typename Visitor>
void visit_bfs(std::shared_ptr<Visitor> visitor,
               std::shared_ptr<Node> source)
{
  std::queue<std::pair<std::shared_ptr<Visitor>,
    std::shared_ptr<Node>>> queue;
  queue.emplace(std::make_pair(visitor, source));
  while (!queue.empty()) {
    auto pair = queue.front();
    queue.pop();
    auto node_copy = pair.first->clone();
    node_copy->process_node(pair.second);
    for (const auto &out_edge_itr: pair.second->get_out_edges()) {
      if (node_copy->should_proceed(pair.second, out_edge_itr.second,
                                    out_edge_itr.first)) {
        auto label_copy = node_copy->clone();
        label_copy->process_label(out_edge_itr.first);
        queue.emplace(std::make_pair(label_copy, out_edge_itr.second));
      }
    }
  }
}

template <typename Iterator>
void visit_unordered(Visitor<Iterator>& visitor, const LabeledGraph& graph)
{
  for (const auto& node: graph.get_nodes()) {
    if (visitor.should_visit_node(node)) {
      visitor.visit_node(node);
    }
    for (const auto& edge: visitor) {
      const auto& [label, dest] = edge;
      if (visitor.should_visit_edge(node, *dest, label)) {
        visitor.visit_edge(node, *dest, label);
      }
    }
  }
}

#endif //CAPS_GRAPH_SEARCH_H
