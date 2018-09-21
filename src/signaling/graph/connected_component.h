//
// Created by smaptas on 25.10.17.
//

#ifndef CAPS_CONNECTED_COMPONENT_H
#define CAPS_CONNECTED_COMPONENT_H

#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "node.h"
#include "visitor.h"

class ConnectedComponent
{
 public:

  explicit ConnectedComponent(std::set<std::shared_ptr<Node>> nodes);

  ConnectedComponent(std::set<std::shared_ptr<Node>> nodes,
                     std::set<std::shared_ptr<Node>> upstream_nodes,
                     std::set<std::shared_ptr<Node>> downstream_nodes);

  int size() const;

  int upstream_size() const;

  int downstream_size() const;

  int num_edges() const;

  std::unordered_map<std::string, int> label_counts() const;

  bool has_node(std::shared_ptr<Node> node) const;

  bool has_upstream_node(std::shared_ptr<Node> node) const;

  bool has_downstream_node(std::shared_ptr<Node> node) const;

  const std::set<std::shared_ptr<Node>> nodes() const;

  const std::set<std::shared_ptr<Node>> upstream_nodes() const;

  const std::set<std::shared_ptr<Node>> downstream_nodes() const;

//  std::vector<std::tuple<std::shared_ptr<Node>, std::shared_ptr<Node>,
//                         std::string>> transitive_paths() const;

 private:

  void count_labels();

  std::set<std::shared_ptr<Node>> nodes_;
  std::set<std::shared_ptr<Node>> upstream_nodes_;
  std::set<std::shared_ptr<Node>> downstream_nodes_;
  std::unordered_map<std::string, int> label_counts_;
  int num_edges_;
};

class TransitivePathVisitor: public CloneableVisitor<TransitivePathVisitor,
                                                     GraphVisitor>
{
 public:
  TransitivePathVisitor(
    std::shared_ptr<Node> source, const ConnectedComponent& component);

  void process_node(const std::shared_ptr<Node>& node) override;

  void process_label(const std::string& label) override;

  bool should_proceed(const std::shared_ptr<Node>& source_node,
                      const std::shared_ptr<Node>& dest_node,
                      const std::string& label) const override;

  static std::vector<std::tuple<std::shared_ptr<Node>, std::shared_ptr<Node>,
                                std::string>> get_paths();

  static void reset_paths();

 private:
  std::shared_ptr<Node> source_;
  std::string label_;
  const ConnectedComponent& component_;
  static std::vector<std::tuple<std::shared_ptr<Node>, std::shared_ptr<Node>,
                                std::string>> paths_;
};

/**
 * Organize a set of nodes into a set of connected components.
 *
 * Given a set of nodes, form a set of connected components based on the
 * edges among the nodes in the set. The set of nodes must be from the same
 * graph. A connected component is a set of nodes in which any two nodes can
 * be reached from one another by following a series of directed edges or
 * reverse directed edges that only pass through nodes in the component.
 *
 * @param nodes - set of nodes from which to make connected components.
 * @return - the set of connected components (as ConnectedComponent instances).
 */
std::vector<ConnectedComponent> make_connected_components(
  std::set<std::shared_ptr<Node>> nodes);

std::vector<std::tuple<std::shared_ptr<Node>, std::shared_ptr<Node>,
                       std::string>> get_transitive_paths(
  const ConnectedComponent& component);

#endif //CAPS_CONNECTED_COMPONENT_H
