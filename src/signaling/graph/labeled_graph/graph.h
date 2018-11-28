#ifndef CAPS_GRAPH_H
#define CAPS_GRAPH_H

// Include C++ standard libraries.
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <queue>

// Include other header files from this project.
#include "../node/node.h"
#include "../../common/pointer_comparator.h"

/**
 * Directed graph with unique root node and string-labeled edges.
 */
class LabeledGraph
{
 public:

  // Alias declarations
  using NodeHandle = Node*;
  using NodeSet = std::set<std::unique_ptr<Node>, pointer_comparator<Node>>;
  using NodeMap = std::unordered_map<NodeHandle, size_t>;
  using LabelMap = std::unordered_map<std::string, size_t>;

  // Constructors and rule of five

  /**
   * Default constructor.
   *
   * Create a LabeledGraph instance with a single root node.
   */
  LabeledGraph();

  /**
   * Copy constructor.
   *
   * Create a LabeledGraph instance from an existing instance. The
   * construction is done via deep copy, that is, all nodes are copied by
   * value and the newly-constructed graph keeps pointers to these copies.
   * Thus in the copy, the node identifiers (e.g., pointers) are distinct
   * from those in the original, but are identical in value.
   *
   * @param orig - the LabeledGraph instance to copy.
   */
  LabeledGraph(const LabeledGraph& orig);

  // Accessors

  /**
   * Get the root node of the graph.
   *
   * @return - a reference to the root node of the graph.
   */
  Node* get_root() const;

  /**
   * Get the number of nodes in the graph with true accept states.
   *
   * @return
   */
  size_t get_num_accept() const;

  size_t get_num_nodes() const;

  size_t get_num_edges() const;

  bool get_compacted() const;

  const NodeSet& get_nodes() const;

  const LabelMap& get_label_counts() const;

  bool has_node(Node* node) const;

  // Operations

  /**
   * Set the accept state of a node in the graph.
   *
   * For `node` in the graph, set its accept state to `accept` (which may be
   * the current accept state of `node`). If `node` is not in the graph, do
   * nothing.
   *
   * @param node - a reference to the node to update.
   * @param accept - the value to which to set the node's accept state.
   */
  void set_accept(Node* node, bool accept);

  NodeHandle add_node(Node* source, const std::string& label);

  /**
   * Remove a node from the graph.
   *
   * Preconditions:
   *   * node is a non-null pointer.
   *   * The node is a member of the graph (i.e., has_node(node) is true).
   *
   * Postconditions:
   *   * The Node instance pointed to by node is destroyed.
   *   * The pointer node is deleted.
   *
   * @param node - the node to delete from the graph.
   */
  void remove_node(Node* node);

  void add_edge(Node* source, Node* destination, const std::string& label);

  NodeHandle add_edge(Node* source, const std::string& label);

  void remove_edge(Node* source, const std::string& label);

 private:

  NodeHandle add_unattached_node();

  void remove_raw_node(Node* node);

  void remove_raw_edge(Node* source, Node* dest, const std::string& label);

  NodeSet nodes_;
  LabelMap label_counts_;
  Node* root_;
  size_t num_accept_;
  size_t num_edges_;
  bool compacted_;

};

bool operator==(const LabeledGraph& lhs, const LabeledGraph& rhs);
bool operator!=(const LabeledGraph& lhs, const LabeledGraph& rhs);

#endif  // CAPS_GRAPH_H
