/**
 * Author: smaptas
 */

#ifndef CAPS_FSALEXICON_H
#define CAPS_FSALEXICON_H

// Include C standard libraries.

// Include C++ standard libraries.

// Include other headers from this project.
#include "lexicon.h"
#include "../graph/node.h"
#include "../graph/graph.h"
#include "../graph/visitor.h"
#include "../common/contains.h"

/**
 * FSA-based implementation of a lexicon.
 */
class FSALexicon: public Lexicon
{
 public:

  //////////////////////////////////////////////////////////////////////////////
  // Constructors and rule of five
  //////////////////////////////////////////////////////////////////////////////

  FSALexicon();

  //////////////////////////////////////////////////////////////////////////////
  // Operations
  //////////////////////////////////////////////////////////////////////////////

  void add_file(std::istream& instream) override;

  void add_string(std::string) override;

  bool has_string(std::string) const override;

  void load(std::istream& instream) override;

  void dump(std::ostream& outstream) const override;

  void compact();

  void compact_long_edges();

  //////////////////////////////////////////////////////////////////////////////
  // Accessors
  //////////////////////////////////////////////////////////////////////////////

  std::set<std::string> dump_strings() const override;

  const LabeledGraph& get_graph() const;

  //////////////////////////////////////////////////////////////////////////////
  // Debugging
  //////////////////////////////////////////////////////////////////////////////

  int register_size() const;

  std::string debug() const;

  std::unordered_map<std::shared_ptr<Node>, int> dest_counts() const;

  std::unordered_map<std::string, int> label_counts() const;

//  void dump_label_huffman(std::ostream& outstream) const;
//
//  int binary_size() const;

 private:

  void set_accept(std::shared_ptr<Node> node, bool accept);

  void replace_or_register(std::shared_ptr<Node> node);

  template <typename NodeFuncType>
  void edit_node(std::shared_ptr<Node> node, NodeFuncType function)
  {
    bool registered = contains(register_, node)
                      && (*register_.find(node) == node);
    if (registered) {
      register_.erase(node);
    }
    function(node);
    if (registered) {
      register_.insert(node);
    }
  }

  LabeledGraph graph_;
  std::unordered_set<std::shared_ptr<Node>, NodeHash, NodePred> register_;

};

class AcceptStringVisitor: public CloneableVisitor<AcceptStringVisitor,
                                                   GraphVisitor>
{
 public:
  AcceptStringVisitor();

  void process_node(const std::shared_ptr<Node>& node) override;

  void process_label(const std::string& label) override;

  std::set<std::string> get_string_dump() const;

  static void reset_strings();

 private:

  std::string label_accumulator_;
  static std::set<std::string> string_dump_;
};

class DebugVisitor: public CloneableVisitor<DebugVisitor, GraphVisitor>,
                    public std::enable_shared_from_this<DebugVisitor>
{
 public:
  DebugVisitor();

  std::shared_ptr<DebugVisitor> clone();

  void process_node(const std::shared_ptr<Node>& node) override;

  std::string get_debug_string() const;

 private:
  std::string debug_string_;
  std::unordered_map<std::shared_ptr<Node>, int> node_map_;
};

class DestinationCountVisitor
  : public CloneableVisitor<DestinationCountVisitor, GraphVisitor>,
    public std::enable_shared_from_this<DestinationCountVisitor>
{
 public:
  DestinationCountVisitor();

  std::shared_ptr<DestinationCountVisitor> clone();

  void process_node(const std::shared_ptr<Node>& node) override;

  std::unordered_map<std::shared_ptr<Node>, int> get_counts() const;

 private:
  std::unordered_map<std::shared_ptr<Node>, int> destination_counts_;
};

class LabelCountVisitor: public CloneableVisitor<LabelCountVisitor,
                                                 GraphVisitor>,
                         public std::enable_shared_from_this<LabelCountVisitor>
{
 public:
  LabelCountVisitor();

  std::shared_ptr<LabelCountVisitor> clone();

  void process_label(const std::string& label) override;

  std::unordered_map<std::string, int> get_counts() const;

 private:
  std::unordered_map<std::string, int> label_counts_;
};

class OrderVisitor: public CloneableVisitor<OrderVisitor, GraphVisitor>,
                    public std::enable_shared_from_this<OrderVisitor>
{
 public:
  OrderVisitor();

  std::shared_ptr<OrderVisitor> clone();

  void process_node(const std::shared_ptr<Node>& node) override;

  const std::unordered_map<std::shared_ptr<Node>, int>& get_order() const;

 private:
  std::unordered_map<std::shared_ptr<Node>, int> node_map_;
};

/**
 * Select a set of nodes based on a function.
 */
class SelectionVisitor: public CloneableVisitor<SelectionVisitor, GraphVisitor>,
                        public std::enable_shared_from_this<SelectionVisitor>
{
 public:

  explicit SelectionVisitor(
    std::function<bool(std::shared_ptr<Node>)> selector
                       = [](std::shared_ptr<Node>) { return true; });

  std::shared_ptr<SelectionVisitor> clone();

  void process_node(const std::shared_ptr<Node>& node) override;

  void reset_selection();

  std::set<std::shared_ptr<Node>> get_selection() const;

 private:

  std::function<bool(std::shared_ptr<Node>)> selector_;
  std::set<std::shared_ptr<Node>> selection_;
};

template <typename ValueType>
class NodeMapVisitor: public CloneableVisitor<NodeMapVisitor<ValueType>,
                                              GraphVisitor>
{
 public:

  explicit NodeMapVisitor(
    std::function<ValueType(std::shared_ptr<Node>)> node_func)
    : node_func_(node_func), map_{}
  {
    // Nothing to do here.
  }

  void process_node(const std::shared_ptr<Node>& node) override
  {
    map_[node] = node_func_(node);
  }

  std::map<std::shared_ptr<Node>, ValueType> get_map() const
  {
    return map_;
  };

 private:

  std::function<ValueType(std::shared_ptr<Node>)> node_func_;
  std::map<std::shared_ptr<Node>, ValueType> map_;
};

template <typename ValueType>
class LabelMapVisitor: public CloneableVisitor<LabelMapVisitor<ValueType>,
                                              GraphVisitor>
{
 public:

  explicit LabelMapVisitor(
    std::function<ValueType(std::string)> label_func)
    : label_func_(label_func), map_{}
  {
    // Nothing to do here.
  }

  void process_label(const std::string& label) override
  {
    map_[label] = label_func_(label);
  }

  std::map<std::shared_ptr<Node>, ValueType> get_map() const
  {
    return map_;
  };

 private:

  std::function<ValueType(std::string)> label_func_;
  std::map<std::shared_ptr<Node>, ValueType> map_;
};

class CompactionVisitor: public CloneableVisitor<CompactionVisitor, GraphVisitor>,
                        public std::enable_shared_from_this<CompactionVisitor>
{
 public:

  CompactionVisitor();

  std::shared_ptr<CompactionVisitor> clone();

  void process_node(const std::shared_ptr<Node>& node) override;

  void reset_selection();

  std::set<std::shared_ptr<Node>> get_selection() const;

 private:

  std::set<std::shared_ptr<Node>> selection_;
};

long size_estimate(int num_labels, int num_edges);

#endif //CAPS_FSALEXICON_H
