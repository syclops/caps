/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#include "connected_component_utils.h"

// Include C standard libraries.

// Include C++ standard libraries.
#include <queue>
#include <unordered_set>

// Include other headers from this project.
#include "../traversal/graph_search.h"
#include "connected_component.h"
#include "transitive_path_visitor.h"

// Include headers from other projects.

std::vector<ConnectedComponent>
make_connected_components(ConnectedComponent::NodeSet nodes)
{
  // Create a set of nodes under consideration consisting only of non-accept
  // nodes in the provided set.
  std::unordered_set<ConnectedComponent::NodeHandle> remaining_nodes;
  for (const auto& node: nodes) {
    if (!node->get_accept()) {
      remaining_nodes.insert(node);
    }
  }
  std::vector<ConnectedComponent> connected_components;
  while (!remaining_nodes.empty()) {
    // Create a component by selecting a random remaining node and taking all
    // remaining nodes transitively connected to that node.
    ConnectedComponent::NodeSet component, upstream, downstream;
    std::queue<ConnectedComponent::NodeHandle> queue;
    queue.push(*remaining_nodes.begin());
    while (!queue.empty()) {
      auto node = queue.front();
      queue.pop();
      component.insert(node);

      for (const auto& [label, parent]: node->get_in_edges()) {
        // Skip if the parent is already in the component or upstream set. No
        // need to check the downstream set since there are no cycles.
        if (component.find(parent) != component.end()
            || upstream.find(parent) != upstream.end()) {
          continue;
        }
        // Determine whether the parent should be part of the component or
        // upstream set.
        if (remaining_nodes.find(parent) == remaining_nodes.end()) {
          upstream.insert(parent);
        } else {
          component.insert(parent);
          queue.push(parent);
        }
      }

      // Do the same as above, but for downstream edges.
      for (const auto& [label, child]: node->get_out_edges()) {
        if (component.find(child) != component.end()
            || downstream.find(child) != downstream.end()) {
          continue;
        }
        if (remaining_nodes.find(child) == remaining_nodes.end()) {
          downstream.insert(child);
        } else {
          component.insert(child);
          queue.push(child);
        }
      }

    }

    // Add the component and erase the nodes it contains from consideration.
    connected_components.emplace_back(component, upstream, downstream);
    for (const auto& node: component) {
      remaining_nodes.erase(node);
    }
  }
  return connected_components;
}

std::vector<TransitivePathVisitor::Path>
get_transitive_paths(const ConnectedComponent& component)
{
  auto visitor = std::make_unique<TransitivePathVisitor>(component);
  for (const auto& node: component.upstream_nodes()) {
    depth_first_search(*visitor, node);
  }
  return visitor->get_result();
}
