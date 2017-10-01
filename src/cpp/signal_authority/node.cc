
#include "node.h"
#include <cstdlib>
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <boost/functional/hash.hpp>

Node::Node()
  : accept_(false), in_degree_(0), edges_()
{
  // Nothing to do here.
}

bool Node::operator==(const Node& rhs) const
{
  return (accept_ == rhs.accept_ && edges_.size() == rhs.edges_.size()
          && std::equal(edges_.begin(), edges_.end(), rhs.edges_.begin()));
}

bool Node::get_accept() const
{
  return accept_;
}

size_t Node::get_in_degree() const
{
  return in_degree_;
}

size_t Node::get_out_degree() const
{
  return edges_.size();
}

std::vector<std::string> Node::get_out_labels() const
{
  std::vector<std::string> out_labels;
  for (auto it = edges_.begin(); it != edges_.end(); ++it) {
    out_labels.push_back(it->first);
  }
  return out_labels;
}

std::vector<Node*> Node::get_out_targets() const
{
  std::vector<Node*> out_targets;
  for (auto it = edges_.begin(); it != edges_.end(); ++it) {
    out_targets.push_back(it->second);
  }
  return out_targets;
}

std::map<std::string, Node*>::const_iterator Node::get_edges_cbegin() const
{
  return edges_.cbegin();
}

std::map<std::string, Node*>::const_iterator Node::get_edges_cend() const
{
  return edges_.cend();
}

std::map<std::string, Node*>::const_reverse_iterator
  Node::get_edges_crbegin() const
{
  return edges_.crbegin();
}

std::map<std::string, Node*>::const_reverse_iterator
  Node::get_edges_crend() const
{
  return edges_.crend();
}

const std::map<std::string, Node*>& Node::get_out_edges() const
{
  return edges_;
}

std::string Node::get_last_key() const
{
  return edges_.rbegin()->first;
}

Node* Node::follow_edge(const std::string label) const
{
  if (edges_.find(label) == edges_.end()) {
    return nullptr;
  }
  return edges_.at(label);
}

void Node::set_accept()
{
  accept_ = true;
}

void Node::copy_properties(const Node* rhs)
{
  accept_ = rhs->accept_;
  in_degree_ = rhs->in_degree_;
}

void Node::add_in_edge()
{
  ++in_degree_;
}

void Node::remove_in_edge()
{
  if (in_degree_ > 0) {
    --in_degree_;
  }
}

void Node::add_out_edge(const std::string label, Node* target)
{
  edges_[label] = target;
}

void Node::remove_out_edge(const std::string label)
{
  edges_.erase(label);
}

std::string Node::to_string() const
{
  std::stringstream repr_stream;
  repr_stream << this;
  if (accept_) {
    repr_stream << "(A)";
  }
  for (auto it = edges_.begin(); it != edges_.end(); ++it) {
    repr_stream << "\n  " << it-> first << " " << it->second;
  }
  return repr_stream.str();
}

size_t NodeHash::operator()(const Node* node) const
{
  size_t seed = 0;
  boost::hash_combine(seed, node->accept_);
  for (auto it = node->edges_.begin(); it != node->edges_.end(); ++it) {
    boost::hash_combine(seed, it->first);
    boost::hash_combine(seed, it->second);
  }
  return seed;
}

bool NodePred::operator()(const Node* node1,
                          const Node* node2) const
{
  return *node1 == *node2;
}
