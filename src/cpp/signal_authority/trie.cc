#include "trie.h"
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include <z3.h>

Trie::Trie(Trie &&orig) noexcept
  : Trie()
{
  swap(*this, orig);
}

Trie& Trie::operator=(Trie rhs)
{
  swap(*this, rhs);
  return *this;
}

void swap(Trie& first, Trie& second)
{
  using std::swap;
  swap(static_cast<GraphBase&>(first), static_cast<GraphBase&>(second));
}

void Trie::add_string(const std::string str)
{
  Node* current_node = root_;
  for (size_t i = 0; i < str.length(); ++i) {
    if (!current_node->follow_edge(str.substr(i, 1))) {
      add_edge(current_node, add_node(), str.substr(i, 1));
    }
    current_node = current_node->follow_edge(str.substr(i, 1));
    if (i == str.length() - 1) {
      set_node_accept(current_node);
    }
  }
  num_bytes_ += str.length();
  ++num_strings_;
}

void Trie::dump_bin(std::ostream& out_stream) const
{
  std::vector<bool> bits;
  for (size_t i = 0; i < 8 - (2 * num_nodes_ + 1) % 8; ++i) {
    bits.push_back(false);
  }
  bits.push_back(true);
  bits.push_back(false);
  std::vector<std::string> labels;
  std::queue<Node*> queue;
  queue.push(root_);
  Node* current_node;
  while (!queue.empty()) {
    current_node = queue.front();
    queue.pop();
    for (auto it = current_node->get_edges_cbegin();
         it != current_node->get_edges_cend(); ++it) {
      bits.push_back(true);
      labels.push_back(it->first);
      queue.push(it->second);
    }
    bits.push_back(false);
  }
  for (size_t i = 0; i < bits.size(); i += 8) {
    char c = 0;
    for (size_t j = 0; j < 8; ++j) {
      c = (c << 1) + bits[i + j];
    }
    out_stream << c;
  }
  for (std::string label: labels) {
    out_stream << '\n' << label;
  }
}

void Trie::load_bin(std::istream& in_stream)
{
  //TODO
}
