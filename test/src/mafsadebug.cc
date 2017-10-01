#include "mafsadebug.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "../../src/cpp/signal_authority/huffman.h"

void DAFSADebug::get_in_degrees(std::unordered_map<Node*, size_t>& map) const
{
  for (Node* node: nodes_) {
    map[node] = node->get_in_degree();
  }
}

void DAFSADebug::pairwise_edges_count(std::ostream &out_stream) const
{
  std::map<size_t, size_t> counts;
  for (Node* source: nodes_) {
    std::unordered_map<Node*, size_t> pair_counts;
    for (Node* target: source->get_out_targets()) {
      if (pair_counts.find(target) == pair_counts.end()) {
        pair_counts[target] = 0;
      }
      ++pair_counts[target];
    }
    for (auto pc: pair_counts) {
      if (counts.find(pc.second) == counts.end()) {
        counts[pc.second] = 0;
      }
      ++counts[pc.second];
    }
  }
  for (auto c: counts) {
    out_stream << c.first << '\t' << c.second << std::endl;
  }
}

void DAFSADebug::topo_diff(std::ostream& out_stream) const
{
  std::unordered_map<Node*, size_t> order = make_order_map(
    get_nodes_topological_order());
  for (Node* node: nodes_) {
    for (auto it = node->get_edges_cbegin(); it != node->get_edges_cend();
         ++it) {
      out_stream << order[it->second] - order[node] << std::endl;
    }
  }
}

void DAFSADebug::print_accept(std::ostream& out_stream) const
{
  std::unordered_set<Node*> accept_set;
  for (Node* node: nodes_) {
    for (Node* target: node->get_out_targets()) {
      if (target->get_accept()) {
        accept_set.insert(target);
      }
    }
  }
  out_stream << "Accepting nodes:" << std::endl;
  for (Node* accept_node: accept_set) {
    out_stream << accept_node << std::endl;
  }
}

void DAFSADebug::print_debug(std::ostream& out_stream) const
{
  std::unordered_map<Node*, size_t> topo_order = make_order_map(
    get_nodes_topological_order());
  std::unordered_map<std::string, size_t> label_counts = get_label_counts();
  HuffmanTree label_code(label_counts);
  std::map<std::string, std::string> label_encoding;
//  label_code.dump_codebook(label_encoding);
  for (Node* source: nodes_) {
    for (auto it: source->get_out_edges()) {
      out_stream << topo_order[source]
                 << " " << topo_order[it.second]
                 << " " << it.first
                 << " " << label_encoding[it.first]
                 << " " << it.second->get_accept()
                 << " " << it.second->get_in_degree()
                 << std::endl;
    }
  }
}

size_t DAFSADebug::accept_count() const
{
  size_t count = 0;
  for (Node* node: nodes_) {
    for (Node* target: node->get_out_targets()) {
      if (target->get_accept()) {
        ++count;
      }
    }
  }
  return count;
}

//size_t DAFSADebug::size_estimate() const
//{
  //Node* final_node = get_final_node();
  //std::unordered_map<std::string, size_t> counts_map = get_label_counts();
  //HuffmanTree label_tree(counts_map);
  //std::unordered_map<std::string, size_t> counts_codebook
    //= label_tree.get_encoding_lengths();
  //size_t size = label_tree.size();
  //for (Node* node: nodes_) {
    //for (auto it: node->get_out_edges()) {
      //size += counts_codebook[it.first] + 3;
      //if (it.second != final_node) {
        //size += ceil(log2(num_nodes_));
      //}
    //}
  //}
  //return size;
//}

Node* DAFSADebug::get_final_node() const
{
  for (Node* node: nodes_) {
    if (node->get_out_degree() == 0) {
      return node;
    }
  }
  return nullptr;
}

std::unordered_set<Node*> DAFSADebug::mark_compactable_nodes(
  const size_t n) const
{
  std::unordered_set<Node*> marked_nodes;
  for (Node* node: nodes_) {
    if (is_node_compactable(node, n)) {
      marked_nodes.insert(node);
    }
  }
  return marked_nodes;
}

std::unordered_set<Node*> DAFSADebug::mark_good_nodes(const size_t n) const
{
  std::unordered_map<Node*, std::unordered_map<std::string, size_t>> in_edges;
  std::unordered_map<std::string, size_t> label_counts = get_label_counts();
  for (Node* source: nodes_) {
    for (auto edge: source->get_out_edges()){
      if (!is_node_compactable(edge.second, n)) {
        continue;
      }
      if (in_edges.find(edge.second) == in_edges.end()) {
        in_edges[edge.second] = std::unordered_map<std::string, size_t>();
      }
      if (in_edges[edge.second].find(edge.first)
          == in_edges[edge.second].end()) {
        in_edges[edge.second][edge.first] = 0;
      }
      ++in_edges[edge.second][edge.first];
    }
  }
  std::unordered_set<Node*> good_nodes;
  for (auto entry: in_edges) {
    if (compact_score(entry.first, entry.second, label_counts) >= 0) {
      good_nodes.insert(entry.first);
    }
  }
  return good_nodes;
}

int DAFSADebug::compact_score(
  Node* node, const std::unordered_map<std::string, size_t>& parent_labels,
  const std::unordered_map<std::string, size_t>& label_counts) const
{
  int score = 0;
  for (auto entry: parent_labels) {
    if (label_counts.at(entry.first) == entry.second) {
      ++score;
    }
    for (auto edge: node->get_out_edges()) {
      if (label_counts.find(edge.first) != label_counts.end()
          && label_counts.at(edge.first) == 1) {
        ++score;
      }
      if (label_counts.find(entry.first + edge.first) == label_counts.end()) {
        --score;
      }
    }
  }
  return score;
}

bool DAFSADebug::is_node_compactable(Node* node, size_t n) const
{
  return (node->get_in_degree() == 1 && node->get_out_degree() <= n)
         || (node->get_in_degree() <= n && node->get_out_degree() == 1);
}

void DAFSADebug::apply_marked_changes(size_t n,
                                      std::unordered_set<Node*>& marked_nodes)
{
  for (Node* node: get_nodes_topological_order()) {
    if (nodes_.find(node) == nodes_.end()) {
      continue;
    }
    for (const std::string& parent_label: node->get_out_labels()) {
      Node* parent_target = node->follow_edge(parent_label);
      if (marked_nodes.find(parent_target) != marked_nodes.end()
          && nodes_.find(parent_target) != nodes_.end()
          && is_node_compactable(parent_target, n)) {
        for (const std::string& child_label: parent_target->get_out_labels()) {
          Node* child_target = parent_target->follow_edge(child_label);
          add_edge(node, child_target, parent_label + child_label);
        }
        remove_edge(node, parent_label);
      }
    }
  }
}

void DAFSADebug::compact_nodes(const size_t n)
{
  std::unordered_set<Node*> marked_nodes = mark_good_nodes(n);
  apply_marked_changes(n, marked_nodes);
}

void DAFSADebug::dump_bin(std::ostream& out_stream) const
{
  // Order the nodes and map each node to its index.
  std::vector<Node*> dfs_order = get_nodes_depth_first_order();
  std::unordered_map<Node*, size_t> node_to_index = make_order_map(dfs_order);
  // Create the Huffman codebook.
  std::unordered_map<std::string, size_t> counts_map = get_label_counts();
  HuffmanTree label_tree(counts_map);
  // Write the canonical Huffman codebook to the stream.
  label_tree.dump_codebook_ascii(out_stream);
  // Write the number of bits used for the destination index to the stream.
  out_stream << (char) ceil(log2(num_nodes_));
  std::vector<bool> bitstream;
  Node* final_node = get_final_node();
  size_t num_destination_bits = get_num_destination_bits(dfs_order, label_tree,
                                                         final_node);
  std::unordered_map<Node*, size_t> bit_indices = get_node_bit_indices(
    dfs_order, label_tree, final_node, num_destination_bits);
  for (Node* node: dfs_order) {
    for (auto it = node->get_edges_cbegin(); it != node->get_edges_cend();
         ++it) {
      for (bool b: label_tree.encode(it->first)) {
        bitstream.push_back(b);
      }
      bitstream.push_back(it->second->get_accept());
      if (it->second->get_accept()) {
        bitstream.push_back(it->second == final_node);
      } else {
        bitstream.push_back(node_to_index[it->second] - node_to_index[node]
                            == 1);
      }
      // Check if this is the final edge of the node
      bitstream.push_back(it->first == node->get_edges_crbegin()->first
                          && it->second == node->get_edges_crbegin()->second);
      // Get destination position
      if (it->second != final_node
          && node_to_index[it->second] - node_to_index[node] != 1) {
        for (bool b: encode_int(bit_indices[it->second],
                                num_destination_bits)) {
          bitstream.push_back(b);
        }
      }
    }
//  for (bool b: label_tree.dump_codebook_bin()) {
//    bitstream.push_back(b);
//  }
  }
  out_stream << bitstream;
}

void DAFSADebug::get_label_changes(
  const size_t n,
  std::map<Node*, std::vector<std::string>>& add_labels,
  std::map<Node*, std::vector<std::string>>& remove_labels,
  std::map<std::string, int>& label_changes) const
{
  std::queue<std::pair<std::string, Node*> > node_queue;
  std::unordered_set<Node*> visited;
  node_queue.push(std::make_pair("", root_));
  visited.insert(root_);
  while (!node_queue.empty()) {
    std::pair<std::string, Node*> current_pair = node_queue.front();
    node_queue.pop();
    std::string prefix = "";
    if (current_pair.second->get_in_degree() == 1
        && current_pair.second->get_out_degree() <= n) {
      // Add node entry to the add and remove tables.
      add_labels[current_pair.second] = std::vector<std::string>();
      remove_labels[current_pair.second] = std::vector<std::string>();
      // Adjust the label counts appropriately.
      prefix = current_pair.first;
      if (label_changes.find(prefix) == label_changes.end()) {
        label_changes[prefix] = 0;
      }
      --label_changes[prefix];
      remove_labels[current_pair.second].push_back(prefix);
      for (std::string label: current_pair.second->get_out_labels()) {
        --label_changes[label];
        remove_labels[current_pair.second].push_back(label);
        if (label_changes.find(prefix + label) == label_changes.end()) {
          label_changes[prefix + label] = 0;
        }
        ++label_changes[prefix + label];
        add_labels[current_pair.second].push_back(prefix + label);
      }
    }
    // Enqueue the child nodes, taking into account recursive prefixes.
    for (auto it: current_pair.second->get_out_edges()) {
      if (visited.find(it.second) == visited.end()) {
        node_queue.push(std::make_pair(prefix + it.first, it.second));
        visited.insert(it.second);
      }
    }
  }
}

void DAFSADebug::get_rev_label_changes(
  const size_t n,
  std::map<Node*, std::vector<std::string>>& add_labels,
  std::map<Node*, std::vector<std::string>>& remove_labels,
  std::map<std::string, int>& label_counts) const
{
  // Create a stack that will pop the nodes in reverse topological order by
  // by using DFS.
  std::stack<Node*> order_stack;
  std::stack<Node*> dfs_stack;
  std::unordered_set<Node*> visited;
  dfs_stack.push(root_);
  Node* current_node;
  while(!dfs_stack.empty()) {
    current_node = dfs_stack.top();
    if (visited.find(current_node) != visited.end()) {
      dfs_stack.pop();
      continue;
    }
    if (all_children_visited(current_node, visited)) {
      dfs_stack.pop();
      order_stack.push(current_node);
      visited.insert(current_node);
    } else {
      for (auto it = current_node->get_edges_crbegin();
           it != current_node->get_edges_crend(); ++it) {
        dfs_stack.push(it->second);
      }
    }
  }
  // Visit the nodes in reverse topological order and populate the proposed
  // change tables.
  std::unordered_map<Node*, std::string> label_map;
  while(!order_stack.empty()) {
    current_node = order_stack.top();
    if (order_stack.top()->get_in_degree() <= n
        && order_stack.top()->get_out_degree() == 1) {
      add_labels[current_node] = std::vector<std::string>();
      remove_labels[current_node] = std::vector<std::string>();
      std::string out_label = current_node->get_out_labels()[0];
      Node* out_target = current_node->follow_edge(out_label);
      label_map[current_node] = out_label;
      if (label_map.find(out_target) != label_map.end()) {
        label_map[current_node] += label_map[out_target];
      }
      if (label_map[current_node] != out_label) {
        remove_labels[current_node].push_back(out_label);
        if (label_counts.find(out_label) == label_counts.end()) {
          label_counts[out_label] = 0;
        }
        --label_counts[out_label];
        add_labels[current_node].push_back(label_map[current_node]);
        if (label_counts.find(label_map[current_node]) == label_counts.end()) {
          label_counts[label_map[current_node]] = 0;
        }
        ++label_counts[label_map[current_node]];
      }
    }
  }
  for (Node* node: nodes_) {
    for (auto it: node->get_out_edges()) {
      if (label_map.find(it.second) != label_map.end()) {
        remove_labels[it.second].push_back(it.first);
      }
    }
  }
}

std::unordered_map<Node*, size_t> DAFSADebug::make_order_map(
  std::vector<Node*> order) const
{
  std::unordered_map<Node*, size_t> order_map;
  for (size_t i = 0; i < order.size(); ++i) {
    order_map[order[i]] = i;
  }
  return order_map;
}

std::vector<Node*> DAFSADebug::get_nodes_breadth_first_order() const
{
  std::vector<Node*> nodes;
  return nodes;
}

std::vector<Node*> DAFSADebug::get_nodes_depth_first_order() const
{
  std::vector<Node*> nodes;
  std::stack<Node*> stack;
  std::unordered_set<Node*> visited;
  stack.push(root_);
  while (!stack.empty()) {
    Node* current_node = stack.top();
    stack.pop();
    for (auto it = current_node->get_edges_crbegin();
         it != current_node->get_edges_crend(); ++it) {
      if (visited.find(it->second) == visited.end()) {
        visited.insert(it->second);
        stack.push(it->second);
      }
    }
    nodes.push_back(current_node);
  }
  return nodes;
}

std::vector<Node*> DAFSADebug::get_nodes_topological_order() const
{
  std::stack<Node*> order_stack;
  std::stack<Node*> dfs_stack;
  std::unordered_set<Node*> visited;
  dfs_stack.push(root_);
  Node* current_node;
  while(!dfs_stack.empty()) {
    current_node = dfs_stack.top();
    if (visited.find(current_node) != visited.end()) {
      dfs_stack.pop();
      continue;
    }
    if (all_children_visited(current_node, visited)) {
      dfs_stack.pop();
      order_stack.push(current_node);
      visited.insert(current_node);
    } else {
      for (auto it = current_node->get_edges_crbegin();
           it != current_node->get_edges_crend(); ++it) {
        dfs_stack.push(it->second);
      }
    }
  }
  std::vector<Node*> ordered_nodes;
  while(!order_stack.empty()) {
    ordered_nodes.push_back(order_stack.top());
    order_stack.pop();
  }
  return ordered_nodes;
}

bool DAFSADebug::all_children_visited(Node* node,
                                 std::unordered_set<Node*>& visited) const
{
  for (Node* child: node->get_out_targets()) {
    if (visited.find(child) == visited.end()) {
      return false;
    }
  }
  return true;
}

size_t DAFSADebug::get_num_destination_bits(const std::vector<Node*>& dfs_order,
                                            const HuffmanTree& label_encoding,
                                            const Node* final_node) const
{
  size_t total_bits = 0;
  size_t long_edge_count = 0;
  for (size_t i = 0; i < dfs_order.size(); ++i) {
    for (auto edge: dfs_order[i]->get_out_edges()) {
      total_bits += label_encoding.encode(edge.first).size() + 3;
      if (edge.second != final_node && i < dfs_order.size() - 1
          && dfs_order[i+1] != edge.second) {
        ++long_edge_count;
      }
    }
  }
  size_t guess = 1;
  while ((1 << guess) < (total_bits + long_edge_count * guess)) {
    ++guess;
  }
  return guess;
}

std::unordered_map<Node*, size_t> DAFSADebug::get_node_bit_indices(
  const std::vector<Node*>& dfs_order, const HuffmanTree& label_encoding,
  const Node* final_node, const size_t num_destination_bits) const
{
  size_t num_bits = 0;
  std::unordered_map<Node*, size_t> bit_indices;
  for (size_t i = 0; i < dfs_order.size(); ++i) {
    bit_indices[dfs_order[i]] = num_bits;
    for (auto edge: dfs_order[i]->get_out_edges()) {
      num_bits += label_encoding.encode(edge.first).size() + 3;
      if (edge.second != final_node && i < dfs_order.size() - 1
          && dfs_order[i+1] != edge.second) {
        num_bits += num_destination_bits;
      }
    }
  }
  return bit_indices;
}

std::ostream& operator<<(std::ostream& out_stream, std::vector<bool>& bitstream)
{
  while (bitstream.size() % 8 != 0) {
    bitstream.push_back(false);
  }
  for (size_t i = 0; i < bitstream.size(); i += 8) {
    int character = 0;
    for (size_t j = 0; j < 8; ++j) {
      character = 2 * character + (int) bitstream[i + j];
    }
    out_stream << (char) character;
  }
  return out_stream;
}
