//
// Created by smaptas on 26.10.17.
//

#ifndef CAPS_GRAPH_SEARCH_H
#define CAPS_GRAPH_SEARCH_H

#include <memory>
#include <queue>
#include <stack>
#include <unordered_set>

#include "graph.h"
#include "node.h"

template <typename Visitor>
void visit_dfs(std::shared_ptr<Visitor> visitor,
               std::shared_ptr<Node> source)
{
  std::stack<std::pair<std::shared_ptr<Visitor>,
    std::shared_ptr<Node>>> stack;
  stack.emplace(std::make_pair(visitor, source));
  std::unordered_set<std::shared_ptr<Node>> visited;
  visited.insert(source);
  while (!stack.empty()) {
    auto pair = stack.top();
    stack.pop();
    auto node_copy = pair.first->clone();
//    std::cerr << "node copy: " << node_copy << std::endl;
//    std::cerr << "target node: " << pair.second << std::endl;
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

template <typename Visitor>
void visit_unordered(std::shared_ptr<Visitor> visitor,
                     const LabeledGraph& graph)
{
  for (auto node: graph.get_nodes()) {
    visitor->process_node(node);
    for (auto out_edge_itr: node->get_out_edges()) {
      visitor->process_label(out_edge_itr.first);
    }
  }
}

#endif //CAPS_GRAPH_SEARCH_H
