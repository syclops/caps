//
// Created by smaptas on 25.10.17.
//

// Include corresponding header file.
#include "connected_component.h"

// Include C++ standard libraries.
#include <iostream>
#include <memory>
#include <queue>
#include <utility>

// Include other header files from this project.
#include "../common/contains.h"
#include "../graph/graph_search.h"

ConnectedComponent::ConnectedComponent(std::set<std::shared_ptr<Node>> nodes)
  : nodes_{std::move(nodes)}, upstream_nodes_{}, downstream_nodes_{},
    label_counts_{}, num_edges_{0}
{
  for (const auto& node: nodes_) {
    num_edges_ += node->get_in_degree() + node->get_out_degree();
    for (auto in_edge: node->get_in_edges()) {
      if (!contains(label_counts_, in_edge.first)) {
        label_counts_.emplace(in_edge.first, 0);
      }
      ++label_counts_[in_edge.first];
      if (!contains(nodes_, in_edge.second.lock())) {
        upstream_nodes_.insert(in_edge.second.lock());
        ++num_edges_;
        ++label_counts_[in_edge.first];
      }
    }
    for (auto out_edge: node->get_out_edges()) {
      if (!contains(label_counts_, out_edge.first)) {
        label_counts_.emplace(out_edge.first, 0);
      }
      ++label_counts_[out_edge.first];
      if (!contains(nodes_, out_edge.second)) {
        downstream_nodes_.insert(out_edge.second);
        ++num_edges_;
        ++label_counts_[out_edge.first];
      }
    }
  }
  num_edges_ /= 2;
  for (auto pair: label_counts_) {
    label_counts_[pair.first] /= 2;
  }
}

ConnectedComponent::ConnectedComponent(
  std::set<std::shared_ptr<Node>> nodes,
  std::set<std::shared_ptr<Node>> upstream_nodes,
  std::set<std::shared_ptr<Node>> downstream_nodes)
  : nodes_{std::move(nodes)}, upstream_nodes_{std::move(upstream_nodes)},
    downstream_nodes_{std::move(downstream_nodes)}, label_counts_{},
    num_edges_{0}
{
  for (const auto& node: nodes_) {
    num_edges_ += node->get_in_degree() + node->get_out_degree();
    for (auto in_edge: node->get_in_edges()) {
      if (!contains(label_counts_, in_edge.first)) {
        label_counts_.emplace(in_edge.first, 0);
      }
      ++label_counts_[in_edge.first];
      if (contains(upstream_nodes_, in_edge.second.lock())) {
        ++num_edges_;
        ++label_counts_[in_edge.first];
      }
    }
    for (auto out_edge: node->get_out_edges()) {
      if (!contains(label_counts_, out_edge.first)) {
        label_counts_.emplace(out_edge.first, 0);
      }
      ++label_counts_[out_edge.first];
      if (contains(downstream_nodes_, out_edge.second)) {
        ++num_edges_;
        ++label_counts_[out_edge.first];
      }
    }
  }
  num_edges_ /= 2;
  for (auto pair: label_counts_) {
    label_counts_[pair.first] /= 2;
  }
}

int ConnectedComponent::size() const
{
  return static_cast<int>(nodes_.size());
}

int ConnectedComponent::upstream_size() const
{
  return static_cast<int>(upstream_nodes_.size());
}

int ConnectedComponent::downstream_size() const
{
  return static_cast<int>(downstream_nodes_.size());
}

int ConnectedComponent::num_edges() const
{
  return num_edges_;
}

std::unordered_map<std::string, int> ConnectedComponent::label_counts() const
{
  return label_counts_;
};

bool ConnectedComponent::has_node(std::shared_ptr<Node> node) const
{
  return nodes_.find(node) != nodes_.end();
}

bool ConnectedComponent::has_upstream_node(std::shared_ptr<Node> node) const
{
  return upstream_nodes_.find(node) != upstream_nodes_.end();
}

bool ConnectedComponent::has_downstream_node(std::shared_ptr<Node> node) const
{
  return downstream_nodes_.find(node) != downstream_nodes_.end();
}

const std::set<std::shared_ptr<Node>> ConnectedComponent::nodes() const
{
  return nodes_;
}

const std::set<std::shared_ptr<Node>> ConnectedComponent::upstream_nodes() const
{
  return upstream_nodes_;
}

const std::set<std::shared_ptr<Node>>
ConnectedComponent::downstream_nodes() const
{
  return downstream_nodes_;
}

std::vector<std::tuple<std::shared_ptr<Node>, std::shared_ptr<Node>,
                       std::string>>
ConnectedComponent::transitive_paths() const
{
  TransitivePathVisitor::reset_paths();
  for (const auto& node: upstream_nodes_) {
    auto visitor = std::make_shared<TransitivePathVisitor>(node,
                                                           downstream_nodes_);
    visit_dfs(visitor, node);
  }
  return TransitivePathVisitor::get_paths();
}

void ConnectedComponent::count_labels()
{
  for (const auto& node: nodes_) {
    num_edges_ += node->get_in_degree() + node->get_out_degree();
    for (auto in_edge: node->get_in_edges()) {
      if (!contains(nodes_, in_edge.second.lock())) {
        upstream_nodes_.insert(in_edge.second.lock());
        ++num_edges_;
      }
    }
    for (auto out_edge: node->get_out_edges()) {
      if (!contains(nodes_, out_edge.second)) {
        downstream_nodes_.insert(out_edge.second);
        ++num_edges_;
      }
    }
  }
  num_edges_ /= 2;
  for (auto pair: label_counts_) {
    label_counts_[pair.first] /= 2;
  }
}

TransitivePathVisitor::TransitivePathVisitor(
  std::shared_ptr<Node> source,
  const std::set<std::shared_ptr<Node>>& downstream_nodes)
  : source_{std::move(source)}, label_{}, downstream_nodes_{downstream_nodes}
{
  // Nothing to do here.
}

void TransitivePathVisitor::process_node(const std::shared_ptr<Node>& node)
{
  if (contains(downstream_nodes_, node)) {
    paths_.emplace_back(source_, node, label_);
  }
}

void TransitivePathVisitor::process_label(const std::string& label)
{
  label_ += label;
}

bool TransitivePathVisitor::should_proceed(const std::shared_ptr<Node>& node,
                                           const std::string& label) const
{
  return !contains(downstream_nodes_, node);
}

std::vector<std::tuple<std::shared_ptr<Node>, std::shared_ptr<Node>,
                       std::string>> TransitivePathVisitor::get_paths()
{
  return paths_;
}

void TransitivePathVisitor::reset_paths()
{
  TransitivePathVisitor::paths_.clear();
}

std::vector<std::tuple<std::shared_ptr<Node>, std::shared_ptr<Node>,
                       std::string>> TransitivePathVisitor::paths_ = {};

std::vector<ConnectedComponent>
make_connected_components(std::set<std::shared_ptr<Node>> nodes)
{
  std::vector<ConnectedComponent> connected_components;
  while (!nodes.empty()) {
    std::set<std::shared_ptr<Node>> component, upstream, downstream;
    std::queue<std::shared_ptr<Node>> queue;
    queue.push(*nodes.begin());
    component.insert(*nodes.begin());
    while (!queue.empty()) {
      auto node = queue.front();
      queue.pop();
      for (auto in_edge: node->get_in_edges()) {
        // Skip the source node if it's already been added to the component or
        // upstream set. No need to check if it's in the downstream set because
        // there are no cycles.
        if (contains(component, in_edge.second.lock())
            || contains(upstream, in_edge.second.lock())) {
          continue;
        }
        // Add the source node to the appropriate set of nodes.
        if (!contains(nodes, in_edge.second.lock())) {
          upstream.insert(in_edge.second.lock());
        } else {
          component.insert(in_edge.second.lock());
          queue.push(in_edge.second.lock());
        }
      }
      for (auto out_edge: node->get_out_edges()) {
        if (contains(component, out_edge.second)
            || contains(downstream, out_edge.second)) {
          continue;
        }
        // Add the destination node to the appropriate set of nodes.
        if (!contains(nodes, out_edge.second)) {
          downstream.insert(out_edge.second);
        } else {
          component.insert(out_edge.second);
          queue.push(out_edge.second);
        }
      }
    }
    for (const auto& component_node: component) {
      nodes.erase(component_node);
    }
    connected_components.emplace_back(component, upstream, downstream);
  }
  return connected_components;
}
