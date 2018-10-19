//
// Created by smaptas on 25.10.17.
//

#ifndef CAPS_CONNECTED_COMPONENT_H
#define CAPS_CONNECTED_COMPONENT_H

#include <cstddef>

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

  /**
   * Return the number of nodes in the component.
   * @return
   */
  size_t size() const;

  /**
   * Return the number of nodes upstream from the component.
   *
   * An upstream node is a node not in the component with a directed edge to
   * a node in the component.
   *
   * @return
   */
  size_t upstream_size() const;

  /**
   * Return the number of nodes downstream from the component.
   *
   * A downstream node is a node not in the component with a directed edge
   * from a node in the component.
   *
   * @return
   */
  size_t downstream_size() const;

  /**
   * Return the number of edges in the component.
   *
   * An edge is considered to be in the component if it begins or ends at a
   * node in the component. Therefore, any edge connecting the component to
   * its upstream or downstream nodes is also considered to be in the component.
   *
   * The number of edges in the component can be used to determine how many
   * edges will be eliminated from the graph if path compaction is applied to
   * the component.
   *
   * @return
   */
  size_t num_edges() const;

  std::unordered_map<std::string, int> label_counts() const;

  bool has_node(std::shared_ptr<Node> node) const;

  bool has_upstream_node(std::shared_ptr<Node> node) const;

  bool has_downstream_node(std::shared_ptr<Node> node) const;

  const std::set<std::shared_ptr<Node>> nodes() const;

  const std::set<std::shared_ptr<Node>> upstream_nodes() const;

  const std::set<std::shared_ptr<Node>> downstream_nodes() const;

 private:

  void count_labels();

  std::set<std::shared_ptr<Node>> nodes_;
  std::set<std::shared_ptr<Node>> upstream_nodes_;
  std::set<std::shared_ptr<Node>> downstream_nodes_;
  std::unordered_map<std::string, int> label_counts_;
  size_t num_edges_;
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
