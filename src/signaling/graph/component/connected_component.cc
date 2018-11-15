//
// Created by smaptas on 25.10.17.
//

// Include corresponding header file.
#include "connected_component.h"

// Include C++ standard libraries.
#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

// Include other header files from this project.
#include "../../common/contains.h"
#include "../traversal/graph_search.h"

ConnectedComponent::ConnectedComponent(ConnectedComponent::NodeSet nodes)
  : nodes_{std::move(nodes)}, num_edges_{0}
{
  set_upstream_nodes();
  set_downstream_nodes();
  set_component_info();
}

ConnectedComponent::ConnectedComponent(ConnectedComponent::NodeSet nodes,
  ConnectedComponent::NodeSet upstream_nodes,
  ConnectedComponent::NodeSet downstream_nodes)
  : nodes_{std::move(nodes)}, upstream_nodes_{std::move(upstream_nodes)},
    downstream_nodes_{std::move(downstream_nodes)}, num_edges_{0}
{
  set_component_info();
}

size_t ConnectedComponent::size() const
{
  return nodes_.size();
}

size_t ConnectedComponent::upstream_size() const
{
  return upstream_nodes_.size();
}

size_t ConnectedComponent::downstream_size() const
{
  return downstream_nodes_.size();
}

size_t ConnectedComponent::num_edges() const
{
  return num_edges_;
}

const ConnectedComponent::LabelCountSet&
ConnectedComponent::label_counts() const
{
  return label_counts_;
}

bool ConnectedComponent::has_node(ConnectedComponent::NodeHandle node) const
{
  return nodes_.find(node) != nodes_.end();
}

bool ConnectedComponent::has_upstream_node(
  ConnectedComponent::NodeHandle node) const
{
  return upstream_nodes_.find(node) != upstream_nodes_.end();
}

bool ConnectedComponent::has_downstream_node(
  ConnectedComponent::NodeHandle node) const
{
  return downstream_nodes_.find(node) != downstream_nodes_.end();
}

const ConnectedComponent::NodeSet& ConnectedComponent::nodes() const
{
  return nodes_;
}

const ConnectedComponent::NodeSet& ConnectedComponent::upstream_nodes() const
{
  return upstream_nodes_;
}

const ConnectedComponent::NodeSet& ConnectedComponent::downstream_nodes() const
{
  return downstream_nodes_;
}

void ConnectedComponent::set_upstream_nodes()
{
  for (const auto& node: nodes_) {
    for (const auto& [label, parent]: node->get_in_edges()) {
      if (nodes_.find(parent) == nodes_.end()) {
        upstream_nodes_.insert(parent);
      }
    }
  }
}

void ConnectedComponent::set_downstream_nodes()
{
  for (const auto& node: nodes_) {
    for (const auto& [label, child]: node->get_out_edges()) {
      if (nodes_.find(child) == nodes_.end()) {
        downstream_nodes_.insert(child);
      }
    }
  }
}

void ConnectedComponent::set_component_info()
{
  for (const auto& node: nodes_) {
    for (auto [label, parent]: node->get_in_edges()) {
      ++label_counts_[label];
      ++num_edges_;
    }
    for (auto [label, child]: node->get_out_edges()) {
      if (nodes_.find(child) == nodes_.end()) {
        ++label_counts_[label];
        ++num_edges_;
      }
    }
  }
}

