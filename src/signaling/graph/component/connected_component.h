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
#include <unordered_set>
#include <vector>

#include "../node/node.h"
#include "../visitor/visitor.h"

/**
 * Class representing a connected component of nodes in a LabeledGraph.
 *
 * A connected component of nodes is a set of nodes in which any two nodes in
 * the set are joined by a sequence of in-edges and out-edges. This does not
 * necessarily mean that any two nodes have a path of directed edges between
 * them. A ConnectedComponent instance stores the component itself, as well
 * as the set of upstream and downstream nodes and information about the
 * component such as label counts.
 */
class ConnectedComponent
{
 public:

  using NodeHandle = const Node*;
  using NodeSet = std::unordered_set<NodeHandle>;
  using LabelCountSet = std::unordered_map<std::string, size_t>;

  explicit ConnectedComponent(NodeSet nodes);

  ConnectedComponent(NodeSet nodes, NodeSet upstream_nodes,
                     NodeSet downstream_nodes);

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

  const LabelCountSet& label_counts() const;

  bool has_node(NodeHandle node) const;

  bool has_upstream_node(NodeHandle node) const;

  bool has_downstream_node(NodeHandle node) const;

  const NodeSet& nodes() const;

  const NodeSet& upstream_nodes() const;

  const NodeSet& downstream_nodes() const;

 private:

  void set_upstream_nodes();

  void set_downstream_nodes();

  void set_component_info();

  NodeSet nodes_;
  NodeSet upstream_nodes_;
  NodeSet downstream_nodes_;
  LabelCountSet label_counts_;
  size_t num_edges_;
};

#endif //CAPS_CONNECTED_COMPONENT_H
