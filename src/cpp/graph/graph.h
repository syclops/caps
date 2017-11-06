#ifndef CAPS_SUCCINCT_TRIE_H_
#define CAPS_SUCCINCT_TRIE_H_

// Include C++ standard libraries.
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

// Include other header files from this project.
#include "../signal_authority/edge.h"
#include "node.h"

class GraphVisitor
{
 public:
  virtual void process_node(const std::shared_ptr<Node>& node);
  virtual void process_label(const std::string& label);
};

class GraphBase
{
 public:

  GraphBase();

  GraphBase(const GraphBase& orig);

  int get_num_nodes() const;

  int get_num_edges() const;

  int get_num_accept() const;

  bool get_compacted() const;

  std::shared_ptr<Node> get_root() const;

  std::shared_ptr<Node> add_node(std::shared_ptr<Node> source,
                                 std::string label);

  void remove_node(std::shared_ptr<Node> node);

  void add_edge(std::shared_ptr<Node> source, std::shared_ptr<Node> destination,
                std::string label);

  std::shared_ptr<Node> add_edge(std::shared_ptr<Node> source,
                                 std::string label);

  void remove_edge(std::shared_ptr<Node> source, std::string label);

  /**
   * Give others access to an immutable iterable of the graph nodes.
   */
  class NodeIterable
  {
    typedef std::set<std::shared_ptr<Node>> nodes_t;

   public:

    explicit NodeIterable(const nodes_t& nodes);

    nodes_t::const_iterator begin() const;

    nodes_t::const_iterator end() const;

   private:
    const nodes_t& nodes_;
  };

  NodeIterable get_nodes() const;

//void GraphBase::dump_dictionary(std::ostream& out_stream) const
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

  void visit_dfs(GraphVisitor& visitor) const;

 private:
  std::shared_ptr<Node> root_;
  int num_accept_;
  int num_edges_;
  bool compacted_;
  std::set<std::shared_ptr<Node>> nodes_;

  std::shared_ptr<Node> add_unattached_node();

//  /**
//   * Swap two GraphBase instances.
//   *
//   * @param first - The first GraphBase instance to swap.
//   * @param second - The second GraphBase instance to swap.
//   */
//  friend void swap(GraphBase& first, GraphBase& second);
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
//  GraphBase(const GraphBase& orig);
//  GraphBase(GraphBase&& orig) noexcept;
//  virtual ~GraphBase();
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

bool operator==(const GraphBase& lhs, const GraphBase& rhs);
bool operator!=(const GraphBase& lhs, const GraphBase& rhs);

#endif  // CAPS_SUCCINCT_TRIE_H_
