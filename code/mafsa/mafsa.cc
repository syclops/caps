#include <cstdio>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "mafsa.h"

Graph::Graph()
  : num_strings_(0), num_states_(1), num_bytes_(0), num_edges_(0), register_()
{
  root_ = new Node;
}

Graph::Graph(const Graph& orig)
  : num_strings_(orig.num_strings_), num_states_(orig.num_states_),
    num_bytes_(orig.num_bytes_), num_edges_(orig.num_edges_),
    register_(orig.register_)
{
  root_ = new Node(*orig.root_);
}

Graph& Graph::operator=(const Graph& rhs)
{
  Graph copy = rhs;
  size_t num_strings_copy = num_strings_;
  num_strings_ = copy.num_strings_;
  copy.num_strings_ = num_strings_copy;
  size_t num_states_copy = num_states_;
  num_states_ = copy.num_states_;
  copy.num_states_ = num_states_copy;
  size_t num_bytes_copy = num_bytes_;
  num_bytes_ = copy.num_bytes_;
  copy.num_bytes_ = num_bytes_copy;
  size_t num_edges_copy = num_edges_;
  num_edges_ = copy.num_edges_;
  copy.num_edges_ = num_edges_copy;
  register_.swap(copy.register_);
  Node* root_copy = root_;
  root_ = copy.root_;
  copy.root_ = root_copy;
  return *this;
}

Graph::~Graph()
{
  std::set<Node*> deletion_list;
  std::queue<Node*> node_queue;
  node_queue.push(root_);
  while (!node_queue.empty()) {
    Node* node = node_queue.front();
    node_queue.pop();
    if (deletion_list.find(node) == deletion_list.end()) {
      deletion_list.insert(node);
      for (std::map<char, Node*>::const_iterator it = node->edges.begin();
           it != node->edges.end(); ++it) {
        node_queue.push(it->second);
      }
    }
  }
  for (std::set<Node*>::iterator it = deletion_list.begin();
       it != deletion_list.end(); ++it) {
    delete *it;
  }
}

void Graph::read_file(std::istream& input_stream)
{
  std::string line;
  while (std::getline(input_stream, line)) {
    add_string(line);
  }
  replace_or_register(root_);
}

size_t Graph::num_strings() const
{
  return num_strings_;
}

size_t Graph::num_states() const
{
  return num_states_;
}

size_t Graph::num_bytes() const
{
  return num_bytes_;
}

size_t Graph::num_edges() const
{
  return num_edges_;
}

std::string Graph::to_string() const
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
    for (std::map<char, Node*>::const_iterator it = node->edges.begin();
         it != node->edges.end(); ++it) {
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

std::string Graph::to_dot() const
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
    for (std::map<char, Node*>::const_iterator it = node->edges.begin();
         it != node->edges.end(); ++it) {
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

void Graph::replace_or_register(Node* node)
{
  char child_char = node->edges.rbegin()->first;
  Node* child = node->edges.rbegin()->second;
  if (!child->edges.empty()) {
    replace_or_register(child);
  }
  for (size_t i = 0; i < register_.size(); ++i) {
    if (*child == *register_[i]) {
      node->edges[child_char] = register_[i];
      num_edges_ -= child->edges.size();
      delete child;
      --num_states_;
      return;
    }
  }
  register_.push_back(child);
}

void Graph::add_string(const std::string str)
{
  //std::cerr << str << std::endl;
  Node* current_node = root_;
  for (size_t i = 0; i < str.length(); ++i) {
    std::map<char, Node*>::iterator find_it = current_node->edges.find(str[i]);
    if (find_it != current_node->edges.end()) {
      current_node = find_it->second;
    } else {
      if (!current_node->edges.empty()) {
        replace_or_register(current_node);
      }
      current_node->edges[str[i]] = new Node;
      ++num_edges_;
      if (i == str.length() - 1) {
        current_node->edges[str[i]]->accept = true;
      }
      ++num_states_;
      current_node = current_node->edges[str[i]];
    }
  }
  num_bytes_ += str.length();
  ++num_strings_;
}

Graph::Node::Node()
  : accept(false), edges()
{
  // Nothing to do here.
}

bool Graph::Node::operator==(const Node& rhs) const
{
  if (accept != rhs.accept) {
    return false;
  }
  std::map<char, Node*>::const_iterator this_it, rhs_it;
  for (this_it = edges.begin(), rhs_it = rhs.edges.begin();
       this_it != edges.end() && rhs_it != rhs.edges.end();
       ++this_it, ++rhs_it) {
    if (this_it->first !=  rhs_it->first || this_it->second != rhs_it->second) {
      return false;
    }
  }
  if (this_it != edges.end() || rhs_it != rhs.edges.end()) {
    return false;
  }
  return true;
}

bool Graph::Node::has_children() const
{
  return !edges.empty();
}

Graph::Node* Graph::Node::last_edge() const
{
  return edges.rbegin()->second;
}

std::string Graph::Node::to_string() const
{
  std::stringstream repr_stream;
  repr_stream << this;
  if (accept) {
    repr_stream << "(A)";
  }
  for (std::map<char, Node*>::const_iterator it = edges.begin();
       it != edges.end(); ++it) {
    repr_stream << "\n  " << it-> first << " " << it->second;
  }
  return repr_stream.str();
}

std::ostream& operator<<(std::ostream& out, const Graph& graph)
{
  return out << graph.to_string();
}
