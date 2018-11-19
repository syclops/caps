/**
 * Author: smaptas
 */

// Include header file.
#include "../../graph/traversal/graph_search.h"
#include "../lexicon.h"
#include "fsa_lexicon.h"

// Include C standard libraries.
#include <cmath>

// Include C++ standard libraries.
#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// Include other headers from this project.
#include "../../common/contains.h"
#include "../../common/powerset.h"
#include "../../encoding/bitvector/bitvector.h"
#include "../../graph/component/connected_component.h"
#include "../../graph/component/connected_component_utils.h"
#include "../../graph/traversal/graph_search.h"
#include "../lexicon.h"
#include "accept_string_visitor.h"

// Include header from other projects.


////////////////////////////////////////////////////////////////////////////////
// FSALexicon methods
////////////////////////////////////////////////////////////////////////////////

FSALexicon::FSALexicon()
  : graph_{}, register_{}
{
  // Nothing to do here.
}

void FSALexicon::add_file(std::istream& instream)
{
  Lexicon::add_file(instream);
  replace_or_register(graph_.get_root());
  register_.clear();
}

void FSALexicon::add_string(const std::string& str)
{
  // If the string is already in the lexicon, do nothing.
  if (has_string(str)) {
    return;
  }

  auto current_node = graph_.get_root();
  for (auto c: str) {
    std::string char_label{c};
    // If the current node does not have an edge labeled c, replace nodes
    // starting from the current node and add a new outgoing edge.
    if (!current_node->has_out_label(char_label)) {
      if (current_node->get_out_degree() > 0) {
        replace_or_register(current_node);
      }
      edit_node(current_node, [this, &char_label](Node* node){
        graph_.add_edge(node, char_label);
      });
    }

    // Advance the node by following the appropriate edge.
    current_node = current_node->follow_out_edge(char_label);
  }

  set_accept(current_node, true);
  ++size_;
}

bool FSALexicon::has_string(const std::string& str) const
{
  unsigned long current_idx = 0;
  auto current_node = graph_.get_root();
  while (current_idx < str.length()) {
    // Match the longest prefix from the current node.
    unsigned long substr_len = 1;
    while (current_node->has_out_label(str.substr(current_idx, substr_len))) {
      ++substr_len;
      if (current_idx + substr_len > str.length()) {
        break;
      }
    }
    if (substr_len == 1) {
      return false;
    }
    current_node = current_node->follow_out_edge(str.substr(current_idx,
                                                            substr_len - 1));
    current_idx += substr_len - 1;
  }
  return current_node->get_accept();
}

void FSALexicon::load(std::istream& instream)
{
  // TODO: edit this
  std::string line;
  while (std::getline(instream, line)) {
    add_string(line);
  }
}

void FSALexicon::dump(std::ostream& outstream) const
{
  for (const auto& str: dump_strings()) {
    outstream << str << "\n";
  }
//  outstream << "tests" << std::endl;
}

std::set<std::string> FSALexicon::dump_strings() const
{
  auto visitor = std::make_unique<AcceptStringVisitor>();
  depth_first_search(*visitor, graph_.get_root());
  return visitor->get_result();
}

void FSALexicon::compact(size_t level)
{
  if (level == 0) {
    return;
  }

  // Compact long paths of 1-in, 1-out nodes.
  compact_long_edges();

  if(level < 2) {
    return;
  }

  // Select all nodes that have an in-degree or out-degree of 1 and are not the
  // source or sink node.
  auto select_candidate = [level](const Node* node) {
      return !node->get_accept()
             && node->get_in_degree() > 0 && node ->get_out_degree() > 0
             && node->get_in_degree() < level && node->get_out_degree() < level
             && (node->get_in_degree() == 1 || node->get_out_degree() == 1);
  };

  // Make connected components out of the selected nodes.
  ConnectedComponent::NodeSet candidates;
  for (const auto& node: graph_.get_nodes()) {
    if (select_candidate(node.get())) {
      candidates.insert(node.get());
    }
  }
  auto candidate_components = make_connected_components(candidates);

  using TransitivePathsSet = std::vector<TransitivePathVisitor::Path>;
  std::vector<std::pair<ConnectedComponent, TransitivePathsSet>> compaction_set;

  // For each component, consider the labels eliminated and introduced.
  const auto& label_counts = graph_.get_label_counts();
  for (const auto& component: candidate_components) {
    auto compaction_score = 0;

    // If a component contains all occurrences of a label in the FSA, it will
    // be eliminated upon compaction.
    for (const auto& [label, count]: component.label_counts()) {
      if (label_counts.at(label) == count) {
        ++compaction_score;
      }
    }

    // Subtract the number of new labels introduced by compacting the component.
    auto paths = get_transitive_paths(component);
    for (const auto& [source, dest, label]: paths) {
      if (label_counts.find(label) == label_counts.end()) {
        --compaction_score;
      }
    }

    if (compaction_score >= 0) {
      compaction_set.emplace_back(component, paths);
    }
  }

  for (const auto& [component, paths]: compaction_set) {
    // Add the new edges that result from compacting the component.
    for (const auto& [source, dest, label]: paths) {
      // By default, TransitivePathVisitor stores the source and destinations
      // of its paths as pointers to const Node. To allow adding
      // the path to the graph, we need to perform const_cast here.
      graph_.add_edge(const_cast<Node*>(source), const_cast<Node*>(dest),
                      label);
    }

    // Remove all upstream edges of the component. After this, once the
    // component goes out of scope, the nodes in it will be removed from the
    // graph.
    for (const auto& upstream_node: component.upstream_nodes()) {

      // Mark any upstream edge for deletion. We do not delete immediately as
      // that would invalidate the upstream node's out-edge iterators.
      std::vector<std::pair<Node*, std::string>> deletion_list;
      for (const auto& [label, dest]: upstream_node->get_out_edges ()) {
        if (component.has_node(dest)) {
          deletion_list.emplace_back(const_cast<Node*>(upstream_node), label);
        }
      }

      // Delete the marked edges from the graph.
      for (const auto& [source, label]: deletion_list) {
        graph_.remove_edge(source, label);
      }
    }
  }
}

void FSALexicon::compact_long_edges()
{
  ConnectedComponent::NodeSet candidates;
  auto select_candidate = [](const Node* node) {
    return node->get_in_degree() == 1 && node->get_out_degree() == 1;
  };
  for (const auto& node: graph_.get_nodes()) {
    if (select_candidate(node.get())) {
      candidates.insert(node.get());
    }
  }
  for (const auto& component: make_connected_components(candidates)) {
    std::cerr << "component!" << std::endl;
    for (const auto& [source, dest, label]: get_transitive_paths(component)) {
      std::cerr << source << ", " << dest << ", " << label << std::endl;
      graph_.add_edge(const_cast<Node*>(source), const_cast<Node*>(dest),
                      label);
    }
    for (const auto& node: component.nodes()) {
      if (graph_.has_node(const_cast<Node*>(node))) {
        graph_.remove_node(const_cast<Node*>(node));
      }
    }
  }
}

//int FSALexicon::register_size() const
//{
//  return static_cast<int>(register_.size());
//}
//
//std::string FSALexicon::debug() const
//{
//  auto visitor = std::make_shared<DebugVisitor>();
//  visit_dfs(visitor, graph_.get_root());
//  return visitor->get_debug_string();
////  std::stringstream debug_string;
////  debug_string << lexicon_.get_num_nodes() << " nodes" << std::endl;
////  debug_string << lexicon_.get_num_accept() << " accept" << std::endl;
////  debug_string << lexicon_.get_num_edges() << " edges" << std::endl;
////  debug_string << std::boolalpha;
////  debug_string << "Compacted: " << lexicon_.get_compacted() << std::endl;
////  return debug_string.str();
//}
//
//std::unordered_map<std::shared_ptr<Node>, int> FSALexicon::dest_counts() const
//{
//  auto visitor = std::make_shared<DestinationCountVisitor>();
//  visit_unordered(visitor, graph_);
//  return visitor->get_counts();
//}
//
//std::unordered_map<std::string, int> FSALexicon::label_counts() const
//{
//  auto visitor = std::make_shared<LabelCountVisitor>();
//  visit_unordered(visitor, graph_);
//  return visitor->get_counts();
//}

const LabeledGraph& FSALexicon::get_graph() const
{
  return graph_;
}

const FSALexicon::Register& FSALexicon::get_register() const
{
  return register_;
}

void FSALexicon::set_accept(Node* node, bool accept)
{
  if (node->get_accept() == accept) {
    return;
  }
  edit_node(node, [this, accept](Node* node_) {
    graph_.set_accept(node_, accept);
  });
}

void FSALexicon::replace_or_register(Node* node)
{
  // If the child node with the lexicographically last label itself has child
  // nodes, then replace or register those nodes first.
  auto last_child = node->get_out_edges().crbegin();
  // Copy the label and child node pointer because the label will get deleted
  // if the child node is replaced.
  auto [child_label, child_node] = *last_child;
  if (child_node->get_out_degree() > 0) {
    replace_or_register(child_node);
  }

  auto registered_node = register_.find(child_node);
  // If an equivalent node is found but it is not the child node, then replace
  // the child node. Otherwise, add the child node to the register.
  if (registered_node != register_.end() && child_node != *registered_node) {
    graph_.remove_node(child_node);
    edit_node(node,
              [&, child_label=child_label](Node* source_node) {
                graph_.add_edge(source_node, *registered_node,
                  child_label);
              });
  } else {
    register_.insert(child_node);
  }
}

void FSALexicon::edit_node(Node* node, std::function<void(Node*)> function)
{
  bool registered = (register_.find(node) != register_.end()
                     && *(register_.find(node)) == node);
  if (registered) {
    register_.erase(node);
  }
  function(node);
  if (registered) {
    register_.insert(node);
  }
}

////////////////////////////////////////////////////////////////////////////////
// DebugVisitor methods
////////////////////////////////////////////////////////////////////////////////

//DebugVisitor::DebugVisitor()
//  : debug_string_{}, node_map_{}
//{
//  // Nothing to do here.
//}
//
//std::shared_ptr<DebugVisitor> DebugVisitor::clone()
//{
//  return shared_from_this();
//}
//
//void DebugVisitor::process_node(const std::shared_ptr<Node>& node)
//{
//  if (!contains(node_map_, node)) {
//    node_map_.emplace(node, node_map_.size());
//  }
//  for (auto edge_itr: node->get_out_edges()) {
//    if (!contains(node_map_, edge_itr.second)) {
//      node_map_.emplace(edge_itr.second, node_map_.size());
//    }
//    std::stringstream debug_stream;
//    debug_stream << debug_string_;
//    debug_stream << node_map_.at(node) << " " << node_map_.at(edge_itr.second)
//                 << " " << edge_itr.first;
//    if (edge_itr.second->get_accept()) {
//      debug_stream << "*";
//    }
//    debug_stream << std::endl;
//    debug_string_ = debug_stream.str();
//  }
//}
//
//std::string DebugVisitor::get_debug_string() const
//{
//  return debug_string_;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//// OrderVisitor methods
//////////////////////////////////////////////////////////////////////////////////
//
//OrderVisitor::OrderVisitor()
//  : node_map_{}
//{
//  // Nothing to do here.
//}
//
//std::shared_ptr<OrderVisitor> OrderVisitor::clone()
//{
//  return shared_from_this();
//}
//
//void OrderVisitor::process_node(const std::shared_ptr<Node>& node)
//{
//  if (!contains(node_map_, node)) {
//    node_map_.emplace(node, node_map_.size());
//  }
//}
//
//const std::unordered_map<std::shared_ptr<Node>, int>&
//OrderVisitor::get_order() const
//{
//  return node_map_;
//}
//
//
//////////////////////////////////////////////////////////////////////////////////
//// CompactionVisitor methods
//////////////////////////////////////////////////////////////////////////////////
//
//CompactionVisitor::CompactionVisitor()
//  : selection_{}
//{
//  // Nothing to do here.
//}
//
//std::shared_ptr<CompactionVisitor> CompactionVisitor::clone()
//{
//  return shared_from_this();
//}
//
//void CompactionVisitor::process_node(const std::shared_ptr<Node>& node)
//{
//  if (node->get_in_degree() > 0 && node->get_out_degree() > 0
//      && (node->get_in_degree() == 1 || node->get_out_degree() == 1)) {
//    selection_.insert(node);
//  }
//}
//
//void CompactionVisitor::reset_selection()
//{
//  selection_.clear();
//}
//
//std::set<std::shared_ptr<Node>> CompactionVisitor::get_selection() const
//{
//  return selection_;
//}
//
//long size_estimate(int num_labels, int num_edges)
//{
//  int guess = 1;
//  while (num_edges * (static_cast<int>(ceil(log2(num_labels))) + guess) > (1 << guess)) {
//    ++guess;
//  }
//  return num_edges * (static_cast<int>(ceil(log2(num_labels))) + guess);
//}
