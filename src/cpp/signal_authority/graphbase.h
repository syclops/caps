#ifndef CAPS_SUCCINCT_TRIE_H_
#define CAPS_SUCCINCT_TRIE_H_

// Include C++ standard libraries.
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// Include other header files from this project.
#include "node.h"

class GraphBase
{
 public:

  /**
   * Swap two GraphBase instances.
   *
   * @param first - The first GraphBase instance to swap.
   * @param second - The second GraphBase instance to swap.
   */
  friend void swap(GraphBase& first, GraphBase& second);

  /**
   * Read a set of strings from a file to add to the dictionary.
   *
   * This method takes an input stream (which can be from standard input, a
   * file, or some other stream) and adds each line (assumed to represent a
   * single word) to the dictionary.
   *
   * @param in_stream - The input stream representing the set of strings to add.
   */
  virtual void read_file(std::istream& in_stream);

  std::string dump_dictionary() const;
  void dump_dictionary(std::ostream& out_stream) const;

  std::string graph_stats() const;
  void graph_stats(std::ostream& out_stream) const;

  std::string dump_dot() const;
  void dump_dot(std::ostream& out_stream) const;

  void dump_txt(std::ostream& out_stream) const;
  void load_txt(std::istream& in_stream);

  std::string dump_label_counts() const;
  void dump_label_counts(std::ostream& out_stream) const;

  std::string dump_target_counts() const;
  void dump_target_counts(std::ostream& out_stream) const;

  bool in_set(std::string str) const;

  void load_dot(std::istream& in_stream);

  virtual std::string dump_bin() const;
  virtual void dump_bin(std::ostream& out_stream) const = 0;

  virtual void load_bin(std::istream& in_stream) = 0;

  void compact_edges();
  void uncompact_edges();

  size_t get_num_strings() const;
  size_t get_num_bytes() const;
  size_t get_num_nodes() const;
  size_t get_num_edges() const;

  std::unordered_map<std::string, size_t> get_label_counts() const;
  std::unordered_map<Node*, size_t> get_target_counts() const;

 protected:

  // Default constructor.
  GraphBase();

  GraphBase(const GraphBase& orig);
  GraphBase(GraphBase&& orig) noexcept;
  virtual ~GraphBase();

  virtual void add_string(std::string str) = 0;

  Node* add_node();
  void remove_node(Node* node);

  void add_edge(Node* source, Node* target, std::string label);
  void remove_edge(Node* source, std::string label);

  void set_node_accept(Node* node);

  void compact_edge(Node* node, std::string label);

  Node* root_;
  size_t num_strings_;
  size_t num_bytes_;
  size_t num_nodes_;
  size_t num_accept_;
  size_t num_edges_;
  bool compacted_;
  std::set<Node*> nodes_;
};

#endif  // CAPS_SUCCINCT_TRIE_H_
