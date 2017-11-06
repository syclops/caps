/**
 * Author: smaptas
 */

// Include header file.
#include "fsa_lexicon.h"

// Include C standard libraries.
#include <cmath>

// Include C++ standard libraries.
#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <string>

// Include other headers from this project.
#include "../common/contains.h"
#include "../common/powerset.h"
#include "../graph/connected_component.h"
#include "../graph/graph_search.h"

// Include header from other projects.

size_t NodeHash::operator()(const std::shared_ptr<Node>& node) const
{
  size_t seed = 0;
  boost::hash_combine(seed, node->get_accept());
  for (auto out_edge_itr: node->get_out_edges()) {
    boost::hash_combine(seed, out_edge_itr.first);
    boost::hash_combine(seed, out_edge_itr.second);
  }
  return seed;
}

bool NodePred::operator()(const std::shared_ptr<Node>& lhs,
                          const std::shared_ptr<Node>& rhs) const
{
  return (lhs->get_accept() == rhs->get_accept()
          && lhs->get_out_degree() == rhs->get_out_degree()
          && std::equal(lhs->get_out_edges().begin(),
                        lhs->get_out_edges().end(),
                        rhs->get_out_edges().begin()));
}

FSALexicon::FSALexicon()
  : graph_{}, register_{}
{
  // Nothing to do here.
}

void FSALexicon::add_file(std::istream& instream)
{
  Lexicon::add_file(instream);
  replace_or_register(graph_.get_root());
}

void FSALexicon::add_string(std::string str)
{
  if (has_string(str)) {
    return;
  }
  auto current_node = graph_.get_root();
  for (auto c: str) {
    if (!current_node->has_out_label(std::string{c})) {
      if (current_node->get_out_degree() > 0) {
        replace_or_register(current_node);
      }
      edit_node(current_node, [this, c](std::shared_ptr<Node> node){
        graph_.add_edge(node, std::string{c});
      });
    }
    current_node = current_node->follow_out_edge(std::string{c});
  }
  set_accept(current_node, true);
  ++size_;
}

bool FSALexicon::has_string(std::string str) const
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
  // TODO: edit this
  for (const auto& str: dump_strings()) {
    outstream << str << "\n";
  }
}

void FSALexicon::finalize()
{
  replace_or_register(graph_.get_root());
}

std::set<std::string> FSALexicon::dump_strings() const
{
  auto visitor = std::make_shared<AcceptStringVisitor>();
  visit_dfs(visitor, graph_.get_root());
  return visitor->get_string_dump();
}

void FSALexicon::compact()
{
  // Select all nodes that have an in-degree or out-degree of 1 and are not the
  // source or sink node.
//  auto candidate_visitor = std::make_shared<SelectionVisitor>(
//    [](const std::shared_ptr<Node>& node) {
//      return node->get_in_degree() > 0 && node->get_out_degree() > 0
//             && node->get_in_degree() == 1 || node->get_out_degree() == 1;
//    });
  std::cerr << "Getting compaction candidates..." << std::flush;
  auto candidate_visitor = std::make_shared<CompactionVisitor>();
  visit_dfs(candidate_visitor, graph_.get_root());
  auto compaction_candidates = candidate_visitor->get_selection();
  candidate_visitor.reset();
  std::cerr << "done! (" << compaction_candidates.size() << " nodes)"
            << std::endl;
  // Collect a set of all of the labels in the graph.
  std::cerr << "Getting current graph stats..." << std::flush;
  auto label_visitor = std::make_shared<LabelCountVisitor>();
  visit_dfs(label_visitor, graph_.get_root());
  auto label_counts = label_visitor->get_counts();
  long uncompacted_size = size_estimate(static_cast<int>(label_counts.size()),
                                        graph_.get_num_edges());
  std::cerr << "done!" << std::endl;
  // For each set in powerset, apply connected components and analyze paths.
  std::set<std::shared_ptr<Node>> min_set;
  long min_size = LONG_MAX;
  long num = 0;
  for (const auto& set: powerset(compaction_candidates)) {
    std::cerr << "Checking set " << num << " (" << set.size() << " elements)" << std::endl;
    std::unordered_map<std::string, int> set_label_counts;
    std::set<std::string> new_labels;
    auto set_edge_count = 0;
//    std::cerr << "This set has " << x.size() << " connected components" << std::endl;
    for (const auto& component: make_connected_components(set)) {
//      std::cerr << "Analyzing component of " << component.size() << " nodes..." << std::endl;
      for (auto pair: component.label_counts()) {
        if (!contains(set_label_counts, pair.first)) {
          set_label_counts.emplace(pair.first, 0);
        }
        set_label_counts[pair.first] += pair.second;
        set_edge_count += pair.second;
      }
      for (auto triple: component.transitive_paths()) {
        new_labels.insert(std::get<2>(triple));
      }
      std::cerr << "done!" << std::endl;
    }
    // Calculate the net effect on the size of the FSA.
//    std::cerr << "Finding new compacted size..." << std::flush;
    int removed_labels = 0;
    for (auto pair: set_label_counts) {
      if (label_counts.at(pair.first) == pair.second) {
        ++removed_labels;
      }
    }
    auto compacted_size = static_cast<long>(label_counts.size() - removed_labels
                                            + new_labels.size());
    if (compacted_size < min_size) {
//      std::cerr << "replacing minimum size..." << std::flush;
      min_size = compacted_size;
      min_set = set;
    }
//    std::cerr << "done!" << std::endl;
    ++num;
  }
  std::cerr << "Starting size: " << uncompacted_size << " bits" << std::endl;
  std::cerr << "Minimum size: " << min_size << " bits" << std::endl;
  auto order_visitor = std::make_shared<OrderVisitor>();
  visit_dfs(order_visitor, graph_.get_root());
  auto order_map = order_visitor->get_order();
  std::cerr << "Original graph: " << std::endl;
  auto debug_visitor = std::make_shared<DebugVisitor>();
  visit_dfs(debug_visitor, graph_.get_root());
  std::cerr << debug_visitor->get_debug_string();
  std::cerr << "Optimal compaction set: {";
  for (const auto& node: min_set) {
    std::cerr << order_map[node] << " ";
  }
  std::cerr << "\b}" << std::endl;
}

std::string FSALexicon::debug() const
{
  auto visitor = std::make_shared<DebugVisitor>();
  visit_dfs(visitor, graph_.get_root());
  return visitor->get_debug_string();
}

void FSALexicon::set_accept(std::shared_ptr<Node> node, bool accept)
{
  if (node->get_accept() == accept) {
    return;
  }
  if (contains(register_, node)) {
    register_.erase(node);
  }
  node->set_accept(accept);

}

void FSALexicon::replace_or_register(std::shared_ptr<Node> node)
{
  auto last_child_itr = node->get_reverse_out_edges().begin();
  auto child_label = last_child_itr->first;
  auto child_node = last_child_itr->second;
  if (child_node->get_out_degree() > 0) {
    replace_or_register(child_node);
  }
  auto registered_node = register_.find(child_node);
  if (registered_node != register_.end()) {
    edit_node(node,
              [this, registered_node, child_label](
                std::shared_ptr<Node> source_node) {
                graph_.add_edge(source_node, *registered_node, child_label);
              });
    graph_.remove_node(child_node);
  } else {
    register_.insert(child_node);
  }
}

AcceptStringVisitor::AcceptStringVisitor()
    : label_accumulator_{}
{
  // Nothing to do here.
}

void AcceptStringVisitor::process_node(
    const std::shared_ptr<Node>& node)
{
  if (node->get_accept()) {
    string_dump_.insert(label_accumulator_);
  }
}

void AcceptStringVisitor::process_label(
    const std::string& label)
{
  label_accumulator_ += label;
}

std::set<std::string> AcceptStringVisitor::get_string_dump() const
{
  return string_dump_;
}

std::set<std::string> AcceptStringVisitor::string_dump_ = {};

DebugVisitor::DebugVisitor()
  : debug_string_{}, node_map_{}
{
  // Nothing to do here.
}

std::shared_ptr<DebugVisitor> DebugVisitor::clone()
{
  return shared_from_this();
}

void DebugVisitor::process_node(const std::shared_ptr<Node>& node)
{
  if (!contains(node_map_, node)) {
    node_map_.emplace(node, node_map_.size());
  }
  for (auto edge_itr: node->get_out_edges()) {
    if (!contains(node_map_, edge_itr.second)) {
      node_map_.emplace(edge_itr.second, node_map_.size());
    }
    std::stringstream debug_stream;
    debug_stream << debug_string_;
    debug_stream << node_map_.at(node) << " " << node_map_.at(edge_itr.second)
                 << " " << edge_itr.first << std::endl;
    debug_string_ = debug_stream.str();
  }
}

std::string DebugVisitor::get_debug_string() const
{
  return debug_string_;
}

LabelCountVisitor::LabelCountVisitor()
  : label_counts_{}
{
  // Nothing to do here.
}

std::shared_ptr<LabelCountVisitor> LabelCountVisitor::clone()
{
  return shared_from_this();
}

void LabelCountVisitor::process_label(const std::string& label)
{
  if (!contains(label_counts_, label)) {
    label_counts_.emplace(label, 0);
  }
  ++label_counts_[label];
}

std::unordered_map<std::string, int> LabelCountVisitor::get_counts() const
{
  return label_counts_;
}

OrderVisitor::OrderVisitor()
  : node_map_{}
{
  // Nothing to do here.
}

std::shared_ptr<OrderVisitor> OrderVisitor::clone()
{
  return shared_from_this();
}

void OrderVisitor::process_node(const std::shared_ptr<Node>& node)
{
  if (!contains(node_map_, node)) {
    node_map_.emplace(node, node_map_.size());
  }
}

const std::unordered_map<std::shared_ptr<Node>, int>&
OrderVisitor::get_order() const
{
  return node_map_;
};

SelectionVisitor::SelectionVisitor()
  : selector_{0}, selection_{}
{
}

SelectionVisitor::SelectionVisitor(
  std::function<bool(const std::shared_ptr<Node>&)> selector)
  : selector_{selector}, selection_{}
{
  // Nothing to do here.
}

std::shared_ptr<SelectionVisitor> SelectionVisitor::clone()
{
  return shared_from_this();
}

void SelectionVisitor::process_node(const std::shared_ptr<Node>& node)
{
  if (selector_ == 0) {
    if (node->get_in_degree() > 0 && node->get_out_degree() > 0
        && (node->get_in_degree() == 1 || node->get_out_degree() == 1)) {
      selection_.insert(node);
    }
  } else if (selector_(node)) {
    selection_.insert(node);
  }
}

void SelectionVisitor::reset_selection()
{
  selection_.clear();
}

std::set<std::shared_ptr<Node>> SelectionVisitor::get_selection() const
{
  return selection_;
}

CompactionVisitor::CompactionVisitor()
  : selection_{}
{
  // Nothing to do here.
}

std::shared_ptr<CompactionVisitor> CompactionVisitor::clone()
{
  return shared_from_this();
}

void CompactionVisitor::process_node(const std::shared_ptr<Node>& node)
{
  if (node->get_in_degree() > 0 && node->get_out_degree() > 0
      && (node->get_in_degree() == 1 || node->get_out_degree() == 1)) {
    selection_.insert(node);
  }
}

void CompactionVisitor::reset_selection()
{
  selection_.clear();
}

std::set<std::shared_ptr<Node>> CompactionVisitor::get_selection() const
{
  return selection_;
}

long size_estimate(int num_labels, int num_edges)
{
  int guess = 1;
  while (num_edges * (static_cast<int>(ceil(log2(num_labels))) + guess) > (1 << guess)) {
    ++guess;
  }
  return num_edges * (static_cast<int>(ceil(log2(num_labels))) + guess);
}