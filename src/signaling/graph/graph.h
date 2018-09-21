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
#include "node.h"

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

 private:
  std::shared_ptr<Node> root_;
  int num_accept_;
  int num_edges_;
  bool compacted_;
  std::set<std::shared_ptr<Node>> nodes_;

  std::shared_ptr<Node> add_unattached_node();

};

bool operator==(const LabeledGraph& lhs, const LabeledGraph& rhs);
bool operator!=(const LabeledGraph& lhs, const LabeledGraph& rhs);

#endif  // CAPS_GRAPH_H
