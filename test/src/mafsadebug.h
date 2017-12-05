/**
 * MAFSA Test Class
 * Author: Stephanos Matsumoto
 */

#ifndef CAPS_MAFSA_TESTER_H
#define CAPS_MAFSA_TESTER_H

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../../src/cpp/encoding/huffman.h"
#include "../../src/cpp/signal_authority/mafsa.h"

class DAFSADebug: public DAFSA
{
 public:
  DAFSADebug() = default;
  DAFSADebug(const DAFSADebug& orig) = default;
  DAFSADebug(DAFSADebug&& orig) noexcept = default;
//  DAFSADebug& operator=(DAFSADebug rhs) = default;
  ~DAFSADebug() = default;

  /**
   * Create a map from nodes to their in-degrees.
   *
   * Create a dictionary that maps each node to its in-degree. This function is
   * useful for determining the relative popularity of destination states in the
   * compact FSA representation.
   *
   * @param map - a reference to the map from nodes to in-degrees.
   */
  void get_in_degrees(std::unordered_map<Node*, size_t>& map) const;

  /**
   * Print the distribution of the number of parallel edges between pairs of
   * nodes to an output stream.
   *
   * Calculate and print the distribution of the number of parallel edges (which
   * share source and destination nodes but have different labels) to an output
   * stream. The distribution is printed as two columns of the form "<number of
   * parallel edges> <count>" and thus the following output means that 128 node
   * pairs have one edge between them, 27 node pairs have 2 parallel edges
   * between them, and 4 node pairs have 3 parallel edges between them:
   *
   * 1 128
   * 2 27
   * 3 4
   *
   * Currently, this information is not exported to a data structure that can be
   * used, nor does it provide any information as to which node pairs have
   * parallel edges. Because we are dealing with a DAFSA, there are no cycles,
   * and more importantly, no node pairs that point to each other. (Our
   * algorithm would *not* consider such node pairs to share parallel edges,
   * because we are technically looking for directed parallel edges.)
   *
   * @param out_stream - a reference to an output stream to which to print the
   *                     distribution information.
   */
  void pairwise_edges_count(std::ostream& out_stream) const;

  /**
   * Assuming nodes are ordered topologically, print the differences in each
   * node pair's indices within the ordering to an output stream.
   *
   * @param out_stream - a reference to an output stream to which to print the
   *                     distribution information.
   */
  void topo_diff(std::ostream& out_stream) const;

  /**
   * Print the set of accept nodes in the graph to an output stream.
   *
   * Determine the set of all accept nodes in the graph and print their
   * addresses to an output stream.
   *
   * Note: This function prints pointers to nodes as a debug feature. As a
   *       result, for most systems, these pointers lose their meaning on
   *       subsequent runs of programs relying on this information, or if the
   *       graph is saved to disk and reloaded.
   *
   * @param out_stream - a reference to an output stream to which to print the
   *                     accept node pointers.
   */
  void print_accept(std::ostream& out_stream) const;

  /**
   * Print various debug information about the graph to an output stream.
   *
   * For all edges in the graph, determine and print the following information:
   *   * the source node index in a topological ordering of the nodes
   *   * the destination node index in a topological ordering of the nodes
   *   * the edge label
   *   * the Huffman encoding of the edge label
   *   * whether the destination node is an accept node
   *   * the in-degree of the destination node
   * This information is printed to the output stream in column format,
   * delimited by spaces.
   *
   * Note: This function is intended to be used for debugging purposes only,
   *       though the output is in a form that can be converted into a table and
   *       processed with other functions.
   *
   * @param out_stream - a reference to an output stream to which to print the
   *                     debug information.
   */
  void print_debug(std::ostream& out_stream) const;

  /**
   * Return the number of accept states in the graph.
   *
   * @return - the number of nodes in the graph whose accept flag is set to
   *           true.
   */
  size_t accept_count() const;

  /**
   */
  //size_t size_estimate() const;

  /**
   * Get a pointer to the final node in the graph, guaranteed to be an accept
   * state with no outgoing edges.
   *
   * @return - a pointer to the final node in the graph.
   */
  Node* get_final_node() const;

  /**
   * Return a set of the n-compactable nodes in the graph.
   *
   * Find all n-compactable nodes in the graph. (A node is called n-compactable
   * if either its in-degree is 1 and its out-degree is at most n, or its
   * in-degree is at most n and its out-degree is 1.)
   *
   * Note: for the purposes of compacting nodes, we assume that the graph
   *       contains no nodes that have an in-degree of 1 and out-degree of 1
   *       (i.e., that the edge compaction process has already been run on this
   *       graph).
   *
   * @param n - the required level of n-compactibility.
   * @return - an STL unordered set of pointers to all n-compactable nodes in
   *           the graph.
   */
  std::unordered_set<Node*> mark_compactable_nodes(size_t n) const;

  /**
   * Return a set of the n-compactable nodes in the graph that do not increase
   * the label count by compacting them.
   *
   * Find all nodes that are n-compactable and do not increase the label count
   * when compacted. For example, if compacting a node would eliminate the
   * labels "foo", "bar", and "baz" from the graph and introduce the new labels
   * "foobar" and "foobaz", the node is included in the set. If on the other
   * hand the labels "foo" and "bar" remained in the graph (i.e., other node
   * pairs have those labels), then the node is not included in the set (since
   * it would eliminate one label but introduce two new ones). Once this has
   * been determined for all nodes, return a set of the "good" nodes.
   *
   * TODO: replace this with the min-cut algorithm discussed.
   *
   * @param n - the level of n-compactability required for a node to be
   *            considered "good".
   */
  std::unordered_set<Node*> mark_good_nodes(size_t n) const;

  /**
   * Calculate the change in the number of labels in the graph by compacting a
   * given node.
   *
   * Given a node, the labels of its incoming edges (with counts, since nodes
   * can have multiple incoming edges with the same label), and the counts of
   * each label in the graph, determine how the number of labels in the graph
   * would change by compacting the node. This function calculates the
   * *reduction* in the number of labels in the graph; therefore, a negative
   * return value indicates an increase in the overall number of unique labels
   * in the graph. For example, if compacting the node eliminates the labels
   * "foo" from the graph and introduces the new label "foobar" and "foobaz",
   * the function returns -1.
   *
   * @param node - a pointer to the node to consider.
   * @param parent_labels - a map of the node's unique incoming labels to the
   *                        number of incoming edges with that label.
   * @param label_counts - a map of labels to how many times they occur in the
   *                       graph.
   * @return - the change in the number of labels in the graph, with a negative
   *           number representing an increase in the number of labels.
   */
  int compact_score(
    Node* node,
    const std::unordered_map<std::string, size_t>& parent_labels,
    const std::unordered_map<std::string, size_t>& label_counts) const;

  /**
   * Test whether a given node is n-compactable.
   *
   * Check whether a node is n-compactable. A node is called n-compactable if
   * either its in-degree is 1 and its out-degree is at most n, or its in-degree
   * is at most n and its out-degree is 1.
   *
   * Note: this function does not check that the node is actually in the graph.
   *
   * @param node - the node to test.
   * @param n - the level of n-compactability to test for.
   * @return - true if node is n-compactable and false otherwise.
   */
  bool is_node_compactable(Node* node, size_t n) const;

  /**
   * Apply compaction to a set of n-compactable nodes, making the changes
   * greedily.
   *
   * Given a compaction level and set of marked nodes, apply compactions to the
   * nodes greedily, proceeding in topological order. If a change causes a
   * marked node to no longer be n-compactable, skip the compaction.
   *
   * @param n - the level of n-compactability.
   * @param marked_nodes - a set of marked n-compactable nodes.
   */
  void apply_marked_changes(size_t n, std::unordered_set<Node*>& marked_nodes);

  /**
   * Compact all n-compactable nodes that do not increase the number of labels
   * in the graph.
   *
   * Find and mark all nodes in the graph that are n-compactable and do not
   * increase the number of labels in the graph. Then, apply the changes
   * greedily, visiting the marked nodes in topological order and compacting
   * them. If by the time a marked node is visited it is no longer n-compactable,
   * do not apply the changes.
   *
   * TODO: don't forget to check its effect on the overall number of labels as
   *       we visit it - perhaps should just do that part as we go through.
   *
   * @param n - the level of n-compactability.
   */
  void compact_nodes(size_t n);

  /**
   * Dump a binary representation of the graph to an output stream.
   *
   * @param out_stream - a reference to an output stream to which to print the
   *                     binary representation.
   */
  void dump_bin(std::ostream& out_stream) const override;

  /**
   * Get forward label changes for n-compactable nodes, taking recursive label
   * changes into account.
   *
   * Calculate the changes to the labels given a set of potential changes by
   * compacting marked nodes, taking recursive label changes into account.
   *
   * @param n - the level of n-compaction.
   * @param add_labels - a map of nodes to the labels they would add upon
   *                     compaction.
   * @param remove_labels - a map of nodes to the labels they would remove upon
   *                        compaction.
   * @param label_changes - a reference to an initialized map in which to place
   *                        the label changes.
   */
  void get_label_changes(
    size_t n, std::map<Node*, std::vector<std::string>>& add_labels,
    std::map<Node*, std::vector<std::string>>& remove_labels,
    std::map<std::string, int>& label_changes) const;

  /**
   * Get forward label changes for n-compactable nodes, taking recursive label
   * changes into account.
   *
   * Calculate the changes to the labels given a set of potential changes by
   * compacting marked nodes, taking recursive label changes into account.
   *
   * @param n - the level of n-compaction.
   * @param add_labels - a map of nodes to the labels they would add upon
   *                     compaction.
   * @param remove_labels - a map of nodes to the labels they would remove upon
   *                        compaction.
   * @param label_changes - a reference to an initialized map in which to place
   *                        the label changes.
   */
  void get_rev_label_changes(
    size_t n, std::map<Node*, std::vector<std::string>>& add_labels,
    std::map<Node*, std::vector<std::string>>& remove_labels,
    std::map<std::string, int>& label_counts) const;

  /**
   * Construct a mapping of node pointers to their index in a given ordering.
   *
   * Given an ordering of nodes, construct a map of nodes to their index in the
   * order, thus allowing for lookup of a node's index given its pointer (i.e.,
   * the inverse of the vector, which allows for lookup of a node pointer based
   * on the index).
   *
   * @param order - a vector of node pointers representing an ordering of nodes.
   * @return - an STL unordered map of node pointers to their order.
   */
  std::unordered_map<Node*, size_t> make_order_map(
    std::vector<Node*> order) const;

  /**
   * Return the set of nodes in breadth-first order.
   *
   * @return - a vector of pointers, one to each node in the graph, in breadth-
   *           first order.
   */
  std::vector<Node*> get_nodes_breadth_first_order() const;

  /**
   * Return the set of nodes in depth-first order.
   *
   * @return - a vector of pointers, one to each node in the graph, in depth-
   *           first order.
   */
  std::vector<Node*> get_nodes_depth_first_order() const;

  /**
   * Return the set of nodes in topological order.
   *
   * @return - a vector of pointers, one to each node in the graph, in
   *           topological order.
   */
  std::vector<Node*> get_nodes_topological_order() const;

  /**
   * Determine if all children of a node have been visited (to help algorithms
   * that rely on checking this property).
   *
   * Given a node in the graph and a set of visited nodes, check to see whether
   * all of the node's child nodes have been visited.
   *
   * @param node - a pointer to the node whose children to check.
   * @param visited - a reference to an STL unordered set of node pointers,
   *                  representing the set of visited nodes.
   */
  bool all_children_visited(Node* node,
                            std::unordered_set<Node*>& visited) const;

  size_t get_num_destination_bits(const std::vector<Node*>& dfs_order,
                                  const HuffmanCoder& label_encoding,
                                  const Node* final_node) const;

  std::unordered_map<Node*, size_t> get_node_bit_indices(
    const std::vector<Node*>& dfs_order, const HuffmanCoder& label_encoding,
    const Node* final_node, const size_t num_destination_bits) const;

  std::vector<std::unordered_set<Node*>> get_contiguous_regions() const;

};

std::ostream& operator<<(std::ostream& out_stream,
                         std::vector<bool>& bitstream);

#endif //CAPS_MAFSA_TESTER_H
