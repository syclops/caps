#ifndef CAPS_SUCCINCT_TRIE_H_
#define CAPS_SUCCINCT_TRIE_H_

// Include C++ standard libraries.
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <queue>

// Include other header files from this project.
#include "node.h"
#include "visitor.h"

class LabeledGraph
{
 public:

  // Constructors and rule of five

  LabeledGraph();

  LabeledGraph(const LabeledGraph& orig);

  // Accessors

  std::shared_ptr<Node> get_root() const;

  int get_num_accept() const;

  int get_num_nodes() const;

  int get_num_edges() const;

  bool get_compacted() const;

  const std::set<std::shared_ptr<Node>>& get_nodes() const;

  // Operations

  void set_accept(std::shared_ptr<Node> node, bool accept);

  std::shared_ptr<Node> add_node(std::shared_ptr<Node> source,
                                 std::string label);

  void remove_node(std::shared_ptr<Node> node);

  void add_edge(std::shared_ptr<Node> source, std::shared_ptr<Node> destination,
                std::string label);

  std::shared_ptr<Node> add_edge(std::shared_ptr<Node> source,
                                 std::string label);

  void remove_edge(std::shared_ptr<Node> source, std::string label);

//void LabeledGraph::dump_dictionary(std::ostream& out_stream) const
//{
//  std::stack<std::pair<std::string, Node*> > pair_stack;
//  pair_stack.push(std::make_pair("", root_));
//  while (!pair_stack.empty()) {
//    auto current_pair = pair_stack.top();
//    std::string current_label = current_pair.first;
//    Node* current_node = current_pair.second;
//    pair_stack.pop();
//    std::vector<std::string> labels = current_node->get_out_labels();
//    for (auto it = current_node->get_edges_crbegin();
//         it != current_node->get_edges_crend(); ++it) {
//      pair_stack.push(std::make_pair(current_label + it->first, it->second));
//    }
//    if (current_node->get_accept()) {
//      out_stream << current_label << std::endl;
//    }
//  }
//}
//

 private:
  std::shared_ptr<Node> root_;
  int num_accept_;
  int num_edges_;
  bool compacted_;
  std::set<std::shared_ptr<Node>> nodes_;

  std::shared_ptr<Node> add_unattached_node();

//  /**
//   * Swap two LabeledGraph instances.
//   *
//   * @param first - The first LabeledGraph instance to swap.
//   * @param second - The second LabeledGraph instance to swap.
//   */
//  friend void swap(LabeledGraph& first, LabeledGraph& second);
//
//  /**
//   * Read a set of strings from a file to add to the dictionary.
//   *
//   * This method takes an input stream (which can be from standard input, a
//   * file, or some other stream) and adds each line (assumed to represent a
//   * single word) to the dictionary.
//   *
//   * @param in_stream - The input stream representing the set of strings to add.
//   */
//  virtual void read_file(std::istream& in_stream);
//
//  std::string dump_dictionary() const;
//  void dump_dictionary(std::ostream& out_stream) const;
//
//  std::string graph_stats() const;
//  void graph_stats(std::ostream& out_stream) const;
//
//  std::string dump_dot() const;
//  void dump_dot(std::ostream& out_stream) const;
//
//  void dump_txt(std::ostream& out_stream) const;
//  void load_txt(std::istream& in_stream);
//
//  std::string dump_label_counts() const;
//  void dump_label_counts(std::ostream& out_stream) const;
//
//  std::string dump_target_counts() const;
//  void dump_target_counts(std::ostream& out_stream) const;
//
//  bool in_set(std::string str) const;
//
//  void load_dot(std::istream& in_stream);
//
//  virtual std::string dump_bin() const;
//  virtual void dump_bin(std::ostream& out_stream) const = 0;
//
//  virtual void load_bin(std::istream& in_stream) = 0;
//
//  void compact_edges();
//  void uncompact_edges();
//
//  size_t get_num_strings() const;
//  size_t get_num_bytes() const;
//  size_t get_num_nodes() const;
//  size_t get_num_edges() const;
//
//  std::unordered_map<std::string, size_t> get_label_counts() const;
//  std::unordered_map<Node*, size_t> get_target_counts() const;
//
// protected:
//
//  LabeledGraph(const LabeledGraph& orig);
//  LabeledGraph(LabeledGraph&& orig) noexcept;
//  virtual ~LabeledGraph();
//
//  virtual void add_string(std::string str) = 0;
//
//  Node* add_node();
//  void remove_node(Node* node);
//
//  void add_edge(Node* source, Node* target, std::string label);
//  void remove_edge(Node* source, std::string label);
//
//  void set_node_accept(Node* node);
//
//  void compact_edge(Node* node, std::string label);

};

bool operator==(const LabeledGraph& lhs, const LabeledGraph& rhs);
bool operator!=(const LabeledGraph& lhs, const LabeledGraph& rhs);

#endif  // CAPS_SUCCINCT_TRIE_H_
