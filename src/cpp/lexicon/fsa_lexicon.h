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

/**
 * Functor to calculate the hash of a node.
 */
struct NodeHash
{
  std::size_t operator()(const std::shared_ptr<Node>& node) const;
};

/**
 * Functor to determine if two node pointers are equivalent.
 */
struct NodePred
{
  bool operator()(const std::shared_ptr<Node>& lhs,
                  const std::shared_ptr<Node>& rhs) const;
};

/**
 * FSA-based implementation of a lexicon.
 */
class FSALexicon: public Lexicon
{
 public:

  FSALexicon();

  void add_file(std::istream& instream) override;

  void add_string(std::string) override;

  bool has_string(std::string) const override;

  void load(std::istream& instream) override;

  void dump(std::ostream& outstream) const override;

  void finalize();

  std::set<std::string> dump_strings() const override;

  void compact();

  std::string debug() const;

 private:

  void set_accept(std::shared_ptr<Node> node, bool accept);

  void replace_or_register(std::shared_ptr<Node> node);

  template <typename NodeFuncType>
  void edit_node(std::shared_ptr<Node> node, NodeFuncType function)
  {
    bool registered = register_.find(node) != register_.end();
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

class SelectionVisitor: public CloneableVisitor<SelectionVisitor, GraphVisitor>,
                        public std::enable_shared_from_this<SelectionVisitor>
{
 public:

  SelectionVisitor();

  explicit SelectionVisitor(
    std::function<bool(const std::shared_ptr<Node>&)> selector);

  std::shared_ptr<SelectionVisitor> clone();

  void process_node(const std::shared_ptr<Node>& node) override;

  void reset_selection();

  std::set<std::shared_ptr<Node>> get_selection() const;

 private:

  const std::function<bool(std::shared_ptr<Node>)>& selector_;
  std::set<std::shared_ptr<Node>> selection_;
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
