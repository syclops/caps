
#include "mafsa.h"
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <queue>
#include "../encoding/huffman.h"

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

DAFSA::DAFSA(const DAFSA& orig)
  : LabeledGraph(orig)
{
  // TODO
}

DAFSA::DAFSA(DAFSA&& orig) noexcept
  : DAFSA()
{
  // Use the "swap trick" to move orig to this instance.
  swap(*this, orig);
}

DAFSA& DAFSA::operator=(DAFSA rhs)
{
  // Use the "swap trick" to handle both copy and move assignment.
  swap(*this, rhs);
  return *this;
}

void swap(DAFSA& first, DAFSA& second)
{
  using std::swap;
  swap(static_cast<LabeledGraph&>(first), static_cast<LabeledGraph&>(second));
  swap(first.register_, second.register_);
}

void DAFSA::read_file(std::istream& in_stream)
{
  LabeledGraph::read_file(in_stream);
  replace_or_register(root_);
}

void DAFSA::dump_bin(std::ostream& out_stream) const
{
  //TODO
}

void DAFSA::load_bin(std::istream &in_stream)
{
  //TODO
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
////////////////////////////////////////////////////////////////////////////////

void DAFSA::replace_or_register(Node* node)
{
  std::string child_label = node->get_edges_crbegin()->first;
  Node* child = node->get_edges_crbegin()->second;
  if (child->get_out_degree()) {
    replace_or_register(child);
  }
  if (register_.find(child) != register_.end()) {
    bool needs_edit = (register_.find(node) != register_.end());
    Node* node_ptr;
    if (needs_edit) {
      node_ptr = *(register_.find(node));
      register_.erase(node);
    }
    add_edge(node, *(register_.find(child)), child_label);
    if (needs_edit) {
      register_.insert(node_ptr);
    }
    remove_node(child);
    return;
  }
  register_.insert(child);
}

void DAFSA::add_string(const std::string str)
{
  Node* current_node = root_;
  for (size_t i = 0; i < str.length(); ++i) {
    if (current_node->follow_edge(str.substr(i, 1))) {
      current_node = current_node->follow_edge(str.substr(i, 1));
    } else {
      // If the current node has children, then we need to replace or register.
      if (current_node->get_out_degree()) {
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
      add_edge(current_node, add_node(), str.substr(i, 1));
      // Reinsert the node if necessary.
      if (needs_edit) {
        register_.insert(node_ptr);
      }
      current_node = current_node->follow_edge(str.substr(i, 1));
    }
  }
  set_node_accept(current_node);
  num_bytes_ += str.length();
  ++num_strings_;
}

