/**
 * Node with accept flag and string-labeled edges
 *
 * Author: Steve Matsumoto
 */

#ifndef CAPS_NODE_H_
#define CAPS_NODE_H_

// Include C++ standard libraries.
#include <map>
#include <memory>
#include <string>

// Include other headers from this project.
#include "../common/iterable.h"

// Include headers from other projects.
#include <boost/functional/hash.hpp>

// Forward declare the Node class for the custom comparator and equality
// predicate.
class Node;

struct in_edge_comparator
{
  bool operator()(const std::pair<std::string, std::weak_ptr<Node>>& lhs,
                  const std::pair<std::string, std::weak_ptr<Node>>& rhs) const;
};

bool in_edge_equal(const std::pair<std::string, std::weak_ptr<Node>>& lhs,
                   const std::pair<std::string, std::weak_ptr<Node>>& rhs);

/**
 * Node with an accept flag and string-labeled, bidirectional edges for use in
 * an appropriate graph.
 */
class Node
{
  friend struct NodeHash;

 private:
  bool accept_;
  std::set<std::pair<std::string, std::weak_ptr<Node>>,
           in_edge_comparator> in_edges_;
  std::map<std::string, std::shared_ptr<Node>> out_edges_;

 public:

  /**
   * Create an empty node.
   */
  Node();

  // The default copy constructor, move constructor, assignment operator, and
  // destructor are fine. A graph composed of instances of this class is
  // responsible for maintaining structural validity (e.g., each node's outgoing
  // edge is matched with an incoming edge to the target node).

  /**
   * Get the node's accept flag value.
   * @return
   */
  bool get_accept() const noexcept;

  /**
   * Get the node's in-degree (i.e., the number of incoming edges).
   * @return
   */
  int get_in_degree() const noexcept;

  /**
   * Get the node's out-degree (i.e., the number of outgoing edges).
   * @return
   */
  int get_out_degree() const noexcept;

  /**
   * Check whether a node has an incoming edge with a given source node and
   * label.
   * @param label
   * @param source
   * @return
   */
  bool has_in_edge(std::string label, std::weak_ptr<Node> source) const;

  /**
   * Check whether a node has an outgoing edge with a given target node and
   * label.
   * @param label
   * @param source
   * @return
   */
  bool has_out_edge(std::string label, std::shared_ptr<Node> source) const;

  /**
   * Check whether a node has an outgoing edge with a given label.
   * @param label
   * @return
   */
  bool has_out_label(std::string label) const;

  /**
   * Set the node's accept flag value.
   * @param accept
   */
  void set_accept(bool accept) noexcept;

  /**
   * Add an incoming edge to a node with a given source node and label.
   * @param label
   * @param source
   */
  void add_in_edge(std::string label, std::weak_ptr<Node> source);

  /**
   * Remove an incoming edge from the node with a given source node and label.
   * @param label
   * @param source
   */
  void remove_in_edge(std::string label, std::weak_ptr<Node> source);

  /**
   * Add an outgoing edge to a node with a given target node and label.
   * @param label
   * @param target
   */
  void add_out_edge(std::string label, std::shared_ptr<Node> target);

  /**
   * Remove an outgoing edge from the node with a given label.
   *
   * Only the label is needed to remove the edge because each node only has a
   * single outgoing edge with a given label at any time.
   * @param label
   */
  void remove_out_edge(std::string label);

  /**
   * Get a pointer to the target node of a node's outgoing edge with a given
   * label.
   * @param label
   * @return
   */
  std::shared_ptr<Node> follow_out_edge(std::string label) const;

  /**
   * Get the set of a node's incoming edges.
   * @return
   */
  const std::set<std::pair<std::string, std::weak_ptr<Node>>,
                 in_edge_comparator>& get_in_edges() const;

  /**
   * Get the set of a node's outgoing edges.
   * @return
   */
  const std::map<std::string, std::shared_ptr<Node>>& get_out_edges() const;

  /**
   * Get the set of a node's incoming edges in reverse order.
   *
   * The edges are ordered first by reverse lexicographical order of the labels
   * and second by reverse order of the source node addresses (which should not
   * be considered a reliable ordering).
   * @return
   */
  ReverseIterable<decltype(in_edges_)> get_reverse_in_edges() const;

  /**
   * Get the set of a node's outgoing edges in reverse order.
   *
   * The edges are ordered by reverse lexicographical order of the labels.
   * @return
   */
  ReverseIterable<decltype(out_edges_)> get_reverse_out_edges() const;
};

/**
 * Test if two nodes are equivalent.
 *
 * Two nodes are considered equivalent if they have the same accept flag value,
 * have the same incoming labels and source nodes associated with each label,
 * and have the same outgoing labels and target nodes associated with each
 * label.
 *
 * @param lhs
 * @param rhs
 * @return
 */
bool operator==(const Node& lhs, const Node& rhs);

/**
 * Test if two nodes are not equivalent.
 *
 * @param lhs
 * @param rhs
 * @return
 */
bool operator!=(const Node& lhs, const Node& rhs);

#endif  // CAPS_NODE_H_
