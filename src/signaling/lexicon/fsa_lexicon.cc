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
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// Include other headers from this project.
#include "../common/contains.h"
#include "../common/powerset.h"
#include "../encoding/huffman.h"
#include "../encoding/bitvector/bitvector.h"
#include "../graph/connected_component.h"
#include "../graph/graph_search.h"

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
}

void FSALexicon::add_string(std::string str)
{
//  std::cerr << "BLAH" << std::endl;
  if (has_string(str)) {
    return;
  }
  auto current_node = graph_.get_root();
  for (auto c: str) {
    if (!current_node->has_out_label(std::string{c})) {
//      std::cerr << "Outgoing label " << c << " not found. Branching from node "
//                << current_node << std::endl;
      if (current_node->get_out_degree() > 0) {
        replace_or_register(current_node);
      }
//      std::cerr << "edit_add " << current_node << " " << c << std::endl;
      edit_node(current_node, [this, c](std::shared_ptr<Node> node){
        graph_.add_edge(node, std::string{c});
      });
    }
    current_node = current_node->follow_out_edge(std::string{c});
//    std::cerr << "Advancing to node " << current_node << std::endl;
  }
//  std::cerr << "set " << current_node << " true" << std::endl;
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
  for (const auto& str: dump_strings()) {
    outstream << str << "\n";
  }
//  outstream << "tests" << std::endl;
}

std::set<std::string> FSALexicon::dump_strings() const
{
  AcceptStringVisitor::reset_strings();
  auto visitor = std::make_shared<AcceptStringVisitor>();
  visit_dfs(visitor, graph_.get_root());
  return visitor->get_string_dump();
}

void FSALexicon::compact()
{
  // Select all nodes that have an in-degree or out-degree of 1 and are not the
  // source or sink node.
  std::cerr << "Getting compaction candidates..." << std::flush;
  auto candidate_visitor = std::make_shared<SelectionVisitor>(
    [](std::shared_ptr<Node> node) {
//      return node->get_in_degree() > 0 && node->get_out_degree() > 0
//             && node->get_in_degree() == 1 || node->get_out_degree() == 1;
      return node->get_in_degree() == 1 && node->get_out_degree() == 1;
    }
  );
  visit_unordered(candidate_visitor, graph_);
  std::cerr << "Selected " << candidate_visitor->get_selection().size()
            << " nodes" << std::endl;
  auto label_count_visitor = std::make_shared<LabelCountVisitor>();
  visit_unordered(label_count_visitor, graph_);
  auto label_counts = label_count_visitor->get_counts();
  std::unordered_map<int, int> score_counts;
  for (const auto& candidate: candidate_visitor->get_selection()) {
    int count = 0;
    for (auto in_edge_itr: candidate->get_in_edges()) {
      if (contains(label_counts, in_edge_itr.first)
          && label_counts.at(in_edge_itr.first) == 1) {
        ++count;
      }
      for (auto out_edge_itr: candidate->get_out_edges()) {
        if (!contains(label_counts, in_edge_itr.first + out_edge_itr.first)) {
          --count;
        }
      }
    }
    for (auto out_edge_itr: candidate->get_out_edges()) {
      if (contains(label_counts, out_edge_itr.first)
          && label_counts.at(out_edge_itr.first) == 1) {
        ++count;
      }
    }
    if (!contains(score_counts, count)) {
      score_counts[count] = 0;
    }
    ++score_counts[count];
  }
  for (auto score_count: score_counts) {
    std::cerr << score_count.first << " " << score_count.second << std::endl;
  }

//  candidate_visitor.reset();
//  std::cerr << "done! (" << compaction_candidates.size() << " nodes)"
//            << std::endl;
//  // Collect a set of all of the labels in the graph.
//  std::cerr << "Getting current graph stats..." << std::flush;
//  auto label_visitor = std::make_shared<LabelCountVisitor>();
//  visit_dfs(label_visitor, lexicon_.get_root());
//  auto label_counts = label_visitor->get_counts();
//  long uncompacted_size = size_estimate(static_cast<int>(label_counts.size()),
//                                        lexicon_.get_num_edges());
//  std::cerr << "done!" << std::endl;
//  // For each set in powerset, apply connected components and analyze paths.
//  std::set<std::shared_ptr<Node>> min_set;
//  long min_size = LONG_MAX;
//  long num = 0;
//  for (const auto& set: powerset(compaction_candidates)) {
//    std::cerr << "Checking set " << num << " (" << set.size() << " elements)" << std::endl;
//    std::unordered_map<std::string, int> set_label_counts;
//    std::set<std::string> new_labels;
//    auto set_edge_count = 0;
////    std::cerr << "This set has " << x.size() << " connected components" << std::endl;
//    for (const auto& component: make_connected_components(set)) {
////      std::cerr << "Analyzing component of " << component.size() << " nodes..." << std::endl;
//      for (auto pair: component.label_counts()) {
//        if (!contains(set_label_counts, pair.first)) {
//          set_label_counts.emplace(pair.first, 0);
//        }
//        set_label_counts[pair.first] += pair.second;
//        set_edge_count += pair.second;
//      }
//      for (auto triple: component.transitive_paths()) {
//        new_labels.insert(std::get<2>(triple));
//      }
//      std::cerr << "done!" << std::endl;
//    }
//    // Calculate the net effect on the size of the FSA.
////    std::cerr << "Finding new compacted size..." << std::flush;
//    int removed_labels = 0;
//    for (auto pair: set_label_counts) {
//      if (label_counts.at(pair.first) == pair.second) {
//        ++removed_labels;
//      }
//    }
//    auto compacted_size = static_cast<long>(label_counts.size() - removed_labels
//                                            + new_labels.size());
//    if (compacted_size < min_size) {
////      std::cerr << "replacing minimum size..." << std::flush;
//      min_size = compacted_size;
//      min_set = set;
//    }
////    std::cerr << "done!" << std::endl;
//    ++num;
//  }
//  std::cerr << "Starting size: " << uncompacted_size << " bits" << std::endl;
//  std::cerr << "Minimum size: " << min_size << " bits" << std::endl;
//  auto order_visitor = std::make_shared<OrderVisitor>();
//  visit_dfs(order_visitor, lexicon_.get_root());
//  auto order_map = order_visitor->get_order();
//  std::cerr << "Original graph: " << std::endl;
//  auto debug_visitor = std::make_shared<DebugVisitor>();
//  visit_dfs(debug_visitor, lexicon_.get_root());
//  std::cerr << debug_visitor->get_debug_string();
//  std::cerr << "Optimal compaction set: {";
//  for (const auto& node: min_set) {
//    std::cerr << order_map[node] << " ";
//  }
//  std::cerr << "\b}" << std::endl;
}

void FSALexicon::compact_long_edges()
{
  auto candidate_visitor = std::make_shared<SelectionVisitor>(
    [](std::shared_ptr<Node> node) {
      return node->get_in_degree() == 1 && node->get_out_degree() == 1;
    }
  );
  visit_unordered(candidate_visitor, graph_);
  for (const auto& component: make_connected_components(
    candidate_visitor->get_selection())) {
    for (auto triple: get_transitive_paths(component)) {
      graph_.add_edge(std::get<0>(triple), std::get<1>(triple),
                      std::get<2>(triple));
    }
    for (const auto& node: component.nodes()) {
      graph_.remove_node(node);
    }
  }
}

int FSALexicon::register_size() const
{
  return static_cast<int>(register_.size());
}

std::string FSALexicon::debug() const
{
  auto visitor = std::make_shared<DebugVisitor>();
  visit_dfs(visitor, graph_.get_root());
  return visitor->get_debug_string();
//  std::stringstream debug_string;
//  debug_string << lexicon_.get_num_nodes() << " nodes" << std::endl;
//  debug_string << lexicon_.get_num_accept() << " accept" << std::endl;
//  debug_string << lexicon_.get_num_edges() << " edges" << std::endl;
//  debug_string << std::boolalpha;
//  debug_string << "Compacted: " << lexicon_.get_compacted() << std::endl;
//  return debug_string.str();
}

std::unordered_map<std::shared_ptr<Node>, int> FSALexicon::dest_counts() const
{
  auto visitor = std::make_shared<DestinationCountVisitor>();
  visit_unordered(visitor, graph_);
  return visitor->get_counts();
}

std::unordered_map<std::string, int> FSALexicon::label_counts() const
{
  auto visitor = std::make_shared<LabelCountVisitor>();
  visit_unordered(visitor, graph_);
  return visitor->get_counts();
}

const LabeledGraph& FSALexicon::get_graph() const
{
  return graph_;
}

void FSALexicon::dump_label_huffman(std::ostream& outstream) const
{
  auto label_count_visitor = std::make_shared<LabelCountVisitor>();
  visit_unordered(label_count_visitor, graph_);
  HuffmanCoder<std::string, BitVector<std::vector<bool>>> coder(label_count_visitor->get_counts());
  auto codebook = coder.decoding_map();
  for (const auto& symbol: std::get<0>(codebook)) {
    outstream << symbol << std::endl;
  }
  for (auto index: std::get<1>(codebook)) {
    outstream << index << " ";
  }
  outstream << std::endl;
  for (auto code: std::get<2>(codebook)) {
    outstream << code << " ";
  }
  outstream << std::endl;
}

int FSALexicon::binary_size() const
{
  auto size = 0;
  auto struct_size = 2 * graph_.get_num_edges();

  auto label_count_map = label_counts();
  HuffmanCoder<std::string, BitVector<std::vector<bool>>> label_coder(label_count_map);
  for (auto pair: label_count_map) {
    struct_size += pair.second * label_coder.encode({pair.first}).size();
    size += 8 * pair.first.length();
  }

  auto dest_count_map = dest_counts();
  HuffmanCoder<std::shared_ptr<Node>, BitVector<std::vector<bool>>> dest_coder
  (dest_count_map);
  for (auto pair: dest_count_map) {
    struct_size += pair.second * dest_coder.encode({pair.first}).size();
  }
  size += ceil(log2(struct_size)) * dest_count_map.size();

  size += struct_size;

  auto label_codebook = label_coder.decoding_map();
  size += std::get<1>(label_codebook).size()
            * ceil(log2(*std::get<1>(label_codebook).rbegin()));
  if (size % 8 != 0) {
    size += 8 - (size % 8);
  }
  size += std::get<2>(label_codebook).size()
          * ceil(log2(*std::get<2>(label_codebook).rbegin()));
  if (size % 8 != 0) {
    size += 8 - (size % 8);
  }

  auto dest_codebook = dest_coder.decoding_map();
  size += std::get<1>(dest_codebook).size()
            * ceil(log2(*std::get<1>(dest_codebook).rbegin()));
  if (size % 8 != 0) {
    size += 8 - (size % 8);
  }
  size +=  std::get<2>(dest_codebook).size()
           * ceil(log2(*std::get<2>(dest_codebook).rbegin()));
  if (size % 8 != 0) {
    size += 8 - (size % 8);
  }

  return size;
}

void FSALexicon::set_accept(std::shared_ptr<Node> node, bool accept)
{
  if (node->get_accept() == accept) {
    return;
  }
  edit_node(node, [this, accept](std::shared_ptr<Node> node_) {
    graph_.set_accept(node_, accept);
  });
}

void FSALexicon::replace_or_register(std::shared_ptr<Node> node)
{
//  std::cerr << "RR " << node << std::endl;
  auto last_child_itr = node->get_reverse_out_edges().begin();
  auto child_label = last_child_itr->first;
  auto child_node = last_child_itr->second;
  if (child_node->get_out_degree() > 0) {
    replace_or_register(child_node);
  }
//  for (auto out_edge_itr: child_node->get_out_edges()) {
//    std::cerr << "  " << out_edge_itr.second << " " << out_edge_itr.first << std::endl;
//  }
  auto registered_node = register_.find(child_node);
  if (registered_node != register_.end() && child_node != *registered_node) {
//    std::cerr << "replace " << child_node << " " << *registered_node << std::endl;
//    std::cerr << "add " << node << " " << *registered_node << " " << child_label << std::endl;
    edit_node(node,
              [this, registered_node, child_label](
                std::shared_ptr<Node> source_node) {
                graph_.add_edge(source_node, *registered_node, child_label);
              });
//    std::cerr << "delete " << child_node << std::endl;
    graph_.remove_node(child_node);
  } else {
//    std::cerr << "register " << child_node << std::endl;
    register_.insert(child_node);
//    std::cerr << "Register: ";
//    for (auto x: register_) {
//      std::cerr << x << " ";
//    }
//    std::cerr << std::endl;
  }
}


////////////////////////////////////////////////////////////////////////////////
// AcceptStringVisitor methods
////////////////////////////////////////////////////////////////////////////////

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

void AcceptStringVisitor::reset_strings()
{
  string_dump_.clear();
}

std::set<std::string> AcceptStringVisitor::string_dump_ = {};


////////////////////////////////////////////////////////////////////////////////
// DebugVisitor methods
////////////////////////////////////////////////////////////////////////////////

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
                 << " " << edge_itr.first;
    if (edge_itr.second->get_accept()) {
      debug_stream << "*";
    }
    debug_stream << std::endl;
    debug_string_ = debug_stream.str();
  }
}

std::string DebugVisitor::get_debug_string() const
{
  return debug_string_;
}


////////////////////////////////////////////////////////////////////////////////
// DestinationCountVisitor methods
////////////////////////////////////////////////////////////////////////////////

DestinationCountVisitor::DestinationCountVisitor()
  : destination_counts_{}
{
  // Nothing to do here.
}

std::shared_ptr<DestinationCountVisitor> DestinationCountVisitor::clone()
{
  return shared_from_this();
}

void DestinationCountVisitor::process_node(const std::shared_ptr<Node>& node)
{
  for (auto itr: node->get_out_edges()) {
    if (!contains(destination_counts_, itr.second)) {
      destination_counts_.emplace(itr.second, 0);
    }
    ++destination_counts_[itr.second];
  }
}

std::unordered_map<std::shared_ptr<Node>, int>
DestinationCountVisitor::get_counts() const
{
  return destination_counts_;
}


////////////////////////////////////////////////////////////////////////////////
// LabelCountVisitor methods
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////
// OrderVisitor methods
////////////////////////////////////////////////////////////////////////////////

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
}


////////////////////////////////////////////////////////////////////////////////
// SelectionVisitor methods
////////////////////////////////////////////////////////////////////////////////

SelectionVisitor::SelectionVisitor(
  std::function<bool(std::shared_ptr<Node>)> selector)
  : selector_{std::move(selector)}, selection_{}
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
    selection_.insert(node);
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


////////////////////////////////////////////////////////////////////////////////
// CompactionVisitor methods
////////////////////////////////////////////////////////////////////////////////

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
