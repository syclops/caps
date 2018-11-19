/**
 * Author: smaptas
 */

#ifndef CAPS_FSALEXICON_H
#define CAPS_FSALEXICON_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

// Include other headers from this project.
#include "../lexicon.h"
#include "../../graph/node/node.h"
#include "../../graph/labeled_graph/graph.h"
#include "../../graph/visitor/visitor.h"
#include "../../common/contains.h"
#include "node_right_language.h"

/**
 * FSA-based implementation of a lexicon.
 */
class FSALexicon: public Lexicon
{
 public:

  // Type alias declarations
  using Register = std::unordered_set<Node*, NodeRightLanguageHash,
                                      NodeRightLanguagePred>;

  //////////////////////////////////////////////////////////////////////////////
  // Constructors and rule of five
  //////////////////////////////////////////////////////////////////////////////

  FSALexicon();

  //////////////////////////////////////////////////////////////////////////////
  // Operations
  //////////////////////////////////////////////////////////////////////////////

  void add_file(std::istream& instream) override;

  void add_string(const std::string& str) override;

  bool has_string(const std::string& str) const override;

  void load(std::istream& instream) override;

  void dump(std::ostream& outstream) const override;

  void compact(size_t level);

  void compact_long_edges();

  //////////////////////////////////////////////////////////////////////////////
  // Accessors
  //////////////////////////////////////////////////////////////////////////////

  std::set<std::string> dump_strings() const override;

  const LabeledGraph& get_graph() const;

  //TODO: for debugging only
  const Register& get_register() const;

  //////////////////////////////////////////////////////////////////////////////
  // Debugging
  //////////////////////////////////////////////////////////////////////////////

//  int register_size() const;
//
//  std::string debug() const;
//
//  std::unordered_map<std::shared_ptr<Node>, int> dest_counts() const;
//
//  std::unordered_map<std::string, int> label_counts() const;

 private:

  void set_accept(Node* node, bool accept);

  void replace_or_register(Node* node);

  void edit_node(Node* node, std::function<void(Node*)> function);

  LabeledGraph graph_;
  Register register_;

};

//class DebugVisitor: public CloneableVisitor<DebugVisitor, GraphVisitor>,
//                    public std::enable_shared_from_this<DebugVisitor>
//{
// public:
//  DebugVisitor();
//
//  std::shared_ptr<DebugVisitor> clone();
//
//  void process_node(const std::shared_ptr<Node>& node) override;
//
//  std::string get_debug_string() const;
//
// private:
//  std::string debug_string_;
//  std::unordered_map<std::shared_ptr<Node>, int> node_map_;
//};
//
//class OrderVisitor: public CloneableVisitor<OrderVisitor, GraphVisitor>,
//                    public std::enable_shared_from_this<OrderVisitor>
//{
// public:
//  OrderVisitor();
//
//  std::shared_ptr<OrderVisitor> clone();
//
//  void process_node(const std::shared_ptr<Node>& node) override;
//
//  const std::unordered_map<std::shared_ptr<Node>, int>& get_order() const;
//
// private:
//  std::unordered_map<std::shared_ptr<Node>, int> node_map_;
//};
//
//template <typename ValueType>
//class NodeMapVisitor: public CloneableVisitor<NodeMapVisitor<ValueType>,
//                                              GraphVisitor>
//{
// public:
//
//  explicit NodeMapVisitor(
//    std::function<ValueType(std::shared_ptr<Node>)> node_func)
//    : node_func_(node_func), map_{}
//  {
//    // Nothing to do here.
//  }
//
//  void process_node(const std::shared_ptr<Node>& node) override
//  {
//    map_[node] = node_func_(node);
//  }
//
//  std::map<std::shared_ptr<Node>, ValueType> get_map() const
//  {
//    return map_;
//  };
//
// private:
//
//  std::function<ValueType(std::shared_ptr<Node>)> node_func_;
//  std::map<std::shared_ptr<Node>, ValueType> map_;
//};
//
//template <typename ValueType>
//class LabelMapVisitor: public CloneableVisitor<LabelMapVisitor<ValueType>,
//                                              GraphVisitor>
//{
// public:
//
//  explicit LabelMapVisitor(
//    std::function<ValueType(std::string)> label_func)
//    : label_func_(label_func), map_{}
//  {
//    // Nothing to do here.
//  }
//
//  void process_label(const std::string& label) override
//  {
//    map_[label] = label_func_(label);
//  }
//
//  std::map<std::shared_ptr<Node>, ValueType> get_map() const
//  {
//    return map_;
//  };
//
// private:
//
//  std::function<ValueType(std::string)> label_func_;
//  std::map<std::shared_ptr<Node>, ValueType> map_;
//};
//
//class CompactionVisitor: public CloneableVisitor<CompactionVisitor, GraphVisitor>,
//                        public std::enable_shared_from_this<CompactionVisitor>
//{
// public:
//
//  CompactionVisitor();
//
//  std::shared_ptr<CompactionVisitor> clone();
//
//  void process_node(const std::shared_ptr<Node>& node) override;
//
//  void reset_selection();
//
//  std::set<std::shared_ptr<Node>> get_selection() const;
//
// private:
//
//  std::set<std::shared_ptr<Node>> selection_;
//};
//
//long size_estimate(int num_labels, int num_edges);

#endif //CAPS_FSALEXICON_H
