#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "mafsa.h"

SignalSet::SignalSet()
  : num_strings_(0), num_bytes_(0), num_states_(1), num_edges_(0),
    compacted_(false), register_()
{
  root_ = new Node;
}

SignalSet::SignalSet(const SignalSet& orig)
  : num_strings_(orig.num_strings_), num_bytes_(orig.num_bytes_),
    num_states_(orig.num_states_), num_edges_(orig.num_edges_),
    compacted_(orig.compacted_), register_(orig.register_)
{
  std::map<Node*, Node*> ptr_map;
  ptr_map.insert(std::pair<Node*, Node*>(orig.root_, new Node));
  std::queue<Node*> node_queue;
  node_queue.push(orig.root_);
  while (!node_queue.empty()) {
    Node* old_node = node_queue.front();
    node_queue.pop();
    Node* new_node = ptr_map[old_node];
    new_node->accept = old_node->accept;
    for (auto it = old_node->edges.begin(); it != old_node->edges.end(); ++it) {
      Node* neighbor = it->second;
      if (ptr_map.find(neighbor) == ptr_map.end()) {
        node_queue.push(neighbor);
        ptr_map.insert(std::pair<Node*, Node*>(neighbor, new Node));
      }
      new_node->join(it->first, ptr_map[neighbor]);
    }
    if (orig.register_.find(old_node) != orig.register_.end()) {
      register_.insert(new_node);
    }
  }
  root_ = ptr_map[orig.root_];
}

SignalSet::SignalSet(SignalSet&& orig)
  : SignalSet()
{
  // Use the "swap trick" to move orig to this instance.
  swap(*this, orig);
}

SignalSet& SignalSet::operator=(SignalSet rhs)
{
  // Use the "swap trick" to handle both copy and move assignment.
  swap(*this, rhs);
  return *this;
}

SignalSet::~SignalSet()
{
  std::set<Node*> deletion_list;
  std::queue<Node*> node_queue;
  node_queue.push(root_);
  while (!node_queue.empty()) {
    Node* node = node_queue.front();
    node_queue.pop();
    if (deletion_list.find(node) == deletion_list.end()) {
      deletion_list.insert(node);
      for (auto it = node->edges.begin(); it != node->edges.end(); ++it) {
        node_queue.push(it->second);
      }
    }
  }
  for (std::set<Node*>::iterator it = deletion_list.begin();
       it != deletion_list.end(); ++it) {
    delete *it;
  }
}

void swap(SignalSet& first, SignalSet& second)
{
  using std::swap;
  swap(first.num_strings_, second.num_strings_);
  swap(first.num_states_, second.num_states_);
  swap(first.num_bytes_, second.num_bytes_);
  swap(first.num_edges_, second.num_edges_);
  swap(first.compacted_, second.compacted_);
  swap(first.register_, second.register_);
  swap(first.root_, second.root_);
}

void SignalSet::read_file(std::istream& input_stream)
{
  std::string line;
  while (std::getline(input_stream, line)) {
    add_string(line);
  }
  replace_or_register(root_);
}

void SignalSet::dump_dictionary(std::ofstream& outfile) const
{
  std::stack<std::pair<std::string, Node*> > pair_stack;
  pair_stack.push(std::make_pair("", root_));
  while (!pair_stack.empty()) {
    auto current_pair = pair_stack.top();
    std::string current_label = current_pair.first;
    Node* current_node = current_pair.second;
    pair_stack.pop();
    for (auto it = current_node->edges.rbegin();
         it != current_node->edges.rend(); ++it) {
      pair_stack.push(std::make_pair(current_label + it->first, it->second));
    }
    if (current_node->accept) {
      outfile << current_label << std::endl;
    }
  }
}

void SignalSet::compact_edges()
{
  std::set<Node*> visited;
  std::queue<Node*> node_queue;
  node_queue.push(root_);
  while (!node_queue.empty()) {
    Node* node = node_queue.front();
    node_queue.pop();
    // Get a list of the uncompacted edge labels.
    std::vector<std::string> labels;
    for (auto it = node->edges.begin(); it != node->edges.end(); ++it) {
      labels.push_back(it->first);
    }
    // Compact each of the edges using the labels obtained above.
    for (size_t i = 0; i < labels.size(); ++i) {
      node->compact_edge(labels[i]);
    }
    // Add each (post-compaction) child node to the queue.
    for (auto it = node->edges.begin(); it != node->edges.end(); ++it) {
      if (visited.find(node) == visited.end()) {
        visited.insert(node);
        node_queue.push(node);
      }
    }
  }
  compacted_ = true;
}

bool SignalSet::check_word(std::string word) const
{
  std::string current_word = word;
  Node* current_node = root_;
  while (current_word.length()) {
    //std::cerr << "Current word: " << current_word << std::endl;
    //std::cerr << "Current node: " << current_node << std::endl;
    std::pair<std::string, Node*> next = match(current_node, current_word);
    //std::cerr << "Got match: " << next.first << ", " << next.second << std::endl;
    if (!next.second) {
      return false;
    } else {
      current_word = current_word.substr(next.first.length(),
                                         std::string::npos);
      current_node = next.second;
    }
  }
  return current_node->accept;
}

size_t SignalSet::num_strings() const
{
  return num_strings_;
}

size_t SignalSet::num_states() const
{
  return num_states_;
}

size_t SignalSet::num_bytes() const
{
  return num_bytes_;
}

size_t SignalSet::num_edges() const
{
  return num_edges_;
}

std::string SignalSet::to_string() const
{
  std::string out_string;
  std::unordered_set<Node*> visited;
  visited.insert(root_);
  std::queue<Node*> node_queue;
  node_queue.push(root_);
  while (!node_queue.empty()) {
    Node* node = node_queue.front();
    node_queue.pop();
    out_string += node->to_string();
    for (auto it = node->edges.begin(); it != node->edges.end(); ++it) {
      if (visited.find(it->second) == visited.end()) {
        visited.insert(it->second);
        node_queue.push(it->second);
      }
    }
    if (!node_queue.empty()) {
      out_string += '\n';
    }
  }
  return out_string;
}

std::string SignalSet::to_dot() const
{
  std::ostringstream out_stream;
  out_stream << "digraph g {" << std::endl;
  std::unordered_map<Node*, int> vertex_num;
  vertex_num[root_] = 0;
  std::queue<Node*> node_queue;
  node_queue.push(root_);
  while (!node_queue.empty()) {
    Node* node = node_queue.front();
    node_queue.pop();
    for (auto it = node->edges.begin(); it != node->edges.end(); ++it) {
      if (vertex_num.find(it->second) == vertex_num.end()) {
        vertex_num[it->second] = vertex_num.size();
        node_queue.push(it->second);
      }
      out_stream << "    " << vertex_num[node] << " -> "
                 << vertex_num[it->second] << " [label=\"" << it->first
                 << "\"];\n";
    }
  }
  out_stream << '}';
  return out_stream.str();
}

void SignalSet::print_dot(std::ofstream& outfile) const
{
  outfile << "digraph g {" << std::endl;
  std::unordered_map<Node*, int> vertex_num;
  vertex_num[root_] = 0;
  std::queue<Node*> node_queue;
  node_queue.push(root_);
  while (!node_queue.empty()) {
    Node* node = node_queue.front();
    node_queue.pop();
    for (auto it = node->edges.begin();
         it != node->edges.end(); ++it) {
      if (vertex_num.find(it->second) == vertex_num.end()) {
        vertex_num[it->second] = vertex_num.size();
        node_queue.push(it->second);
      }
      outfile << "    " << vertex_num[node] << " -> "
                 << vertex_num[it->second] << " [label=\"" << it->first
                 << "\"];" << std::endl;
    }
  }
  outfile << '}';
}
void SignalSet::replace_or_register(Node* node)
{
  std::string child_string = node->edges.rbegin()->first;
  Node* child = node->edges.rbegin()->second;
  if (!child->edges.empty()) {
    replace_or_register(child);
  }
  if (register_.find(child) != register_.end()) {
    bool needs_edit = (register_.find(node) != register_.end());
    Node* node_ptr;
    if (needs_edit) {
      node_ptr = *(register_.find(node));
      register_.erase(node);
    }
    node->join(child_string, *(register_.find(child)));
    num_edges_ -= child->edges.size();
    if (needs_edit) {
      register_.insert(node_ptr);
    }
    delete child;
    --num_states_;
    return;
  }
  register_.insert(child);
}

void SignalSet::add_string(const std::string str)
{
  Node* current_node = root_;
  for (size_t i = 0; i < str.length(); ++i) {
    // If an edge matches, move to the node incident to that edge.
    std::string label = str.substr(i, 1);
    if (current_node->edges.find(label) != current_node->edges.end()) {
      current_node = current_node->edges[label];
    } else {
      // If the current node has children, then we need to replace or register.
      if (!current_node->edges.empty()) {
        replace_or_register(current_node);
      }
      // The node needs to be removed, edited, and reinserted if in the
      // register.
      bool needs_edit = (register_.find(current_node) != register_.end());
      Node* node_ptr;
      if (needs_edit) {
        node_ptr = *(register_.find(current_node));
        register_.erase(current_node);
      }
      // Add new node and edge to that node.
      current_node->join(label, new Node);
      ++num_states_;
      ++num_edges_;
      // If this is the end of the string, set the state to be accepting.
      if (i == str.length() - 1) {
        current_node->edges[label]->accept = true;
      }
      // Reinsert the node if necessary.
      if (needs_edit) {
        register_.insert(node_ptr);
      }
      current_node = current_node->edges[label];
    }
  }
  num_bytes_ += str.length();
  ++num_strings_;
}

std::pair<std::string, SignalSet::Node*> SignalSet::match(
  Node* node, std::string word) const
{
  //std::cerr << "Entering match" << std::endl;
  for (auto it = node->edges.begin(); it != node->edges.end(); ++it) {
    auto match = std::mismatch(it->first.begin(), it->first.end(),
                               word.begin());
    //std::cerr << "it: " << it->first << ", " << it->second << std::endl;
    if (match.first == it->first.end()) {
      return *it;
    }
  }
  //std::cerr << "No match found" << std::endl;
  return std::pair<std::string, Node*>("", nullptr);
}

SignalSet::Node::Node()
  : accept(false), in_degree(0), edges()
{
  // Nothing to do here.
}

bool SignalSet::Node::operator==(const Node& rhs) const
{
  return (accept == rhs.accept && edges.size() == rhs.edges.size()
          && std::equal(edges.begin(), edges.end(), rhs.edges.begin()));
}

bool SignalSet::Node::has_children() const
{
  return !edges.empty();
}

SignalSet::Node* SignalSet::Node::last_edge() const
{
  return edges.rbegin()->second;
}

std::string SignalSet::Node::to_string() const
{
  std::stringstream repr_stream;
  repr_stream << this;
  if (accept) {
    repr_stream << "(A)";
  }
  for (auto it = edges.begin(); it != edges.end(); ++it) {
    repr_stream << "\n  " << it-> first << " " << it->second;
  }
  return repr_stream.str();
}

inline void SignalSet::Node::join(std::string label, Node* target)
{
  edges[label] = target;
  ++target->in_degree;
}

void SignalSet::Node::compact_edge(const std::string label)
{
  std::string current_label(label);
  Node* current_target = edges.at(label);
  std::vector<Node*> deletion_list;
  while (current_target->in_degree == 1 && current_target->edges.size() == 1) {
    deletion_list.push_back(current_target);
    current_label += current_target->edges.begin()->first;
    current_target = current_target->edges.begin()->second;
  }
  for (size_t i = 0; i < deletion_list.size(); ++i) {
    delete deletion_list[i];
  }
  join(current_label, current_target);
}

std::ostream& operator<<(std::ostream& out, const SignalSet& graph)
{
  return out << graph.to_string();
}
