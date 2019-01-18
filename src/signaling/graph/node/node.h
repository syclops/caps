/**
 * Node with accept flag and string-labeled edges
 *
 * Author: Steve Matsumoto
 */

#ifndef CAPS_NODE_H_
#define CAPS_NODE_H_

#define DEBUG

// Include C++ standard libraries.
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>

// Include other headers from this project.
#include "../../common/iterable.h"

// Include headers from other projects.
#include <boost/functional/hash.hpp>

/**
 * Node with an accept flag and string-labeled, bidirectional edges for use in
 * an appropriate graph.
 */
class Node
{
 public:

  // Alias declarations
  using NodeHandle = Node*;
  using Label = std::string;
  using HalfEdge = std::pair<Label, NodeHandle>;
  using InEdgeSet = std::set<HalfEdge>;
  using OutEdgeSet = std::map<Label, NodeHandle>;

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
  size_t get_in_degree() const noexcept;

  /**
   * Get the node's out-degree (i.e., the number of outgoing edges).
   * @return
   */
  size_t get_out_degree() const noexcept;

  /**
   * Check whether a node has an incoming edge with a given source node and
   * label.
   * @param label
   * @param source
   * @return
   */
  bool has_in_edge(const Label& label, Node* source) const;

  /**
   * Check whether a node has an outgoing edge with a given target node and
   * label.
   * @param label
   * @param source
   * @return
   */
  bool has_out_edge(const Label& label, const Node* source) const;

  /**
   * Check whether a node has an outgoing edge with a given label.
   * @param label
   * @return
   */
  bool has_out_label(const Label& label) const;

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
  void add_in_edge(const Label& label, Node* source);

  /**
   * Remove an incoming edge from the node with a given source node and label.
   *
   * NOTE: the lack of const for source is deliberate, and has to do with the
   *       way that sets guarantee const-ness when finding or erasing
   *       elements. Specifically, the pointer's const-ness is guaranteed,
   *       but not the const-ness of the node pointed to. There is probably a
   *       safer way to do this, but it is in here for now.
   *
   * @param label
   * @param source
   */
  void remove_in_edge(const Label& label, Node* source);

  /**
   * Add an outgoing edge to a node with a given target node and label.
   * @param label
   * @param target
   */
  void add_out_edge(const Label& label, Node* target);

  /**
   * Remove an outgoing edge from the node with a given label.
   *
   * Only the label is needed to remove the edge because each node only has a
   * single outgoing edge with a given label at any time.
   * @param label
   */
  void remove_out_edge(const Label& label);

  /**
   * Get a pointer to the target node of a node's outgoing edge with a given
   * label.
   * @param label
   * @return
   */
  Node* follow_out_edge(const Label& label) const;

  /**
   * Get the set of a node's incoming edges.
   * @return
   */
  const std::set<HalfEdge>& get_in_edges() const;

  /**
   * Get the set of a node's outgoing edges.
   * @return
   */
  const std::map<Label, Node*>& get_out_edges() const;

  /**
   * Get the set of a node's incoming edges in reverse order.
   *
   * The edges are ordered first by reverse lexicographical order of the labels
   * and second by reverse order of the source node addresses (which should not
   * be considered a reliable ordering).
   * @return
   */
  ReverseIterable<InEdgeSet> get_reverse_in_edges() const;

  /**
   * Get the set of a node's outgoing edges in reverse order.
   *
   * The edges are ordered by reverse lexicographical order of the labels.
   * @return
   */
  ReverseIterable<OutEdgeSet> get_reverse_out_edges() const;

#ifdef DEBUG
  unsigned int id;
  static unsigned int id_ctr;
#endif

  std::string get_id() const;

 private:
  bool accept_;
  InEdgeSet in_edges_;
  OutEdgeSet out_edges_;
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

namespace std {
  template<> struct hash<Node>
  {
    size_t operator()(const Node& node) const
    {
      size_t seed = 0;
      boost::hash_combine(seed, node.get_accept());
      for (const auto& [label, parent]: node.get_in_edges()) {
        boost::hash_combine(seed, label);
        boost::hash_combine(seed, parent);
      }
      for (const auto& [label, child]: node.get_out_edges()) {
        boost::hash_combine(seed, label);
        boost::hash_combine(seed, child);
      }
      return seed;
    }
  };

  template<> struct hash<reference_wrapper<Node>>
  {
    size_t operator()(const reference_wrapper<Node>& wrapper) const
    {
      return hash<Node>()(wrapper.get());
    }
  };

  template<> struct hash<reference_wrapper<const Node>>
  {
    size_t operator()(const reference_wrapper<const Node>& wrapper) const
    {
      return hash<Node>()(wrapper.get());
    }
  };
}

#endif  // CAPS_NODE_H_
