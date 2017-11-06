
#include "node.h"
#include <cstdlib>
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <boost/functional/hash.hpp>

bool in_edge_comparator::operator()(
  const std::pair<std::string, std::weak_ptr<Node>>& lhs,
  const std::pair<std::string, std::weak_ptr<Node>>& rhs) const
{
  return lhs.first < rhs.first
         || (lhs.first == rhs.first
             && std::owner_less<std::weak_ptr<Node>>()(lhs.second,
                                                       rhs.second));
}

bool in_edge_equal(const std::pair<std::string, std::weak_ptr<Node>>& lhs,
                   const std::pair<std::string, std::weak_ptr<Node>>& rhs)
{
  return lhs.first == rhs.first && lhs.second.lock() == rhs.second.lock();
}

Node::Node()
  : accept_{false}, in_edges_{}, out_edges_{}
{
  // Nothing to do here.
}

//bool Node::operator==(const Node& rhs) const
//{
//  return (accept_ == rhs.accept_ && in_edges_.size() == rhs.in_edges_.size()
//          && out_edges_.size() == rhs.out_edges_.size()
//          && std::equal(in_edges_.begin(), in_edges_.end(),
//                        rhs.in_edges_.begin())
//          && std::equal(out_edges_.begin(), out_edges_.end(),
//                        rhs.out_edges_.begin()));
//}

bool Node::get_accept() const noexcept
{
  return accept_;
}

int Node::get_in_degree() const noexcept
{
  return static_cast<int>(in_edges_.size());
}

int Node::get_out_degree() const noexcept
{
  return static_cast<int>(out_edges_.size());
}

std::string Node::get_last_out_label() const
{
  return get_reverse_out_edges().begin()->first;
}

bool Node::has_in_edge(std::string label, std::weak_ptr<Node> source) const
{
  return in_edges_.find(std::make_pair(label, source)) != in_edges_.end();
}

bool Node::has_out_edge(std::string label, std::shared_ptr<Node> target) const
{
  return follow_out_edge(label) == target;
}

bool Node::has_out_label(std::string label) const
{
  return out_edges_.find(label) != out_edges_.end();
}

//std::vector<std::string> Node::get_out_labels() const
//{
//  std::vector<std::string> out_labels;
//  for (auto it = out_edges_.begin(); it != out_edges_.end(); ++it) {
//    out_labels.push_back(it->first);
//  }
//  return out_labels;
//}
//
//std::vector<Node*> Node::get_out_targets() const
//{
//  std::vector<Node*> out_targets;
//  for (auto it = out_edges_.begin(); it != out_edges_.end(); ++it) {
//    out_targets.push_back(it->second);
//  }
//  return out_targets;
//}
//
//std::map<std::string, Node*>::const_iterator Node::get_edges_cbegin() const
//{
//  return out_edges_.cbegin();
//}
//
//std::map<std::string, Node*>::const_iterator Node::get_edges_cend() const
//{
//  return out_edges_.cend();
//}
//
//std::map<std::string, Node*>::const_reverse_iterator
//  Node::get_edges_crbegin() const
//{
//  return out_edges_.crbegin();
//}
//
//std::map<std::string, Node*>::const_reverse_iterator
//  Node::get_edges_crend() const
//{
//  return out_edges_.crend();
//}
//
//const std::map<std::string, Node*>& Node::get_out_edges() const
//{
//  return out_edges_;
//}
//
//std::string Node::get_last_key() const
//{
//  return out_edges_.rbegin()->first;
//}
//
//Node* Node::follow_edge(const std::string label) const
//{
//  if (out_edges_.find(label) == out_edges_.end()) {
//    return nullptr;
//  }
//  return out_edges_.at(label);
//}

void Node::set_accept(bool accept) noexcept
{
  accept_ = accept;
}

//void Node::copy_properties(const Node* rhs)
//{
//  accept_ = rhs->accept_;
//}

void Node::add_in_edge(std::string label, std::weak_ptr<Node> source)
{
  in_edges_.emplace(std::make_pair(label, source));
}

void Node::remove_in_edge(std::string label, std::weak_ptr<Node> source)
{
  in_edges_.erase(std::make_pair(label, source));
}

void Node::add_out_edge(std::string label, std::shared_ptr<Node> target)
{
  out_edges_.emplace(label, target);
}

void Node::remove_out_edge(std::string label)
{
  out_edges_.erase(label);
}

//void Node::clear_in_edges()
//{
//  in_edges_.clear();
//}
//
//void Node::clear_out_edges()
//{
//  out_edges_.clear();
//}
//
//std::weak_ptr<Node> Node::follow_in_edge(std::string label) const
//{
//  if (in_edges_.find(label) != in_edges_.end()) {
//    return in_edges_.at(label);
//  } else {
//    return {};
//  }
//}

std::shared_ptr<Node> Node::follow_out_edge(std::string label) const
{
  return has_out_label(label) ? out_edges_.at(label) : std::shared_ptr<Node>();
}

Node::EdgeIterable<std::set<std::pair<std::string, std::weak_ptr<Node>>,
                            in_edge_comparator>>
Node::get_in_edges() const
{
  return {in_edges_};
}

Node::EdgeIterable<std::map<std::string, std::shared_ptr<Node>>>
Node::get_out_edges() const
{
  return {out_edges_};
}

Node::EdgeReverseIterable<std::set<std::pair<std::string, std::weak_ptr<Node>>,
                                   in_edge_comparator>>
Node::get_reverse_in_edges() const
{
  return {in_edges_};
}

Node::EdgeReverseIterable<std::map<std::string, std::shared_ptr<Node>>>
Node::get_reverse_out_edges() const
{
  return {out_edges_};
}

//std::string Node::to_string() const
//{
//  std::stringstream repr_stream;
//  repr_stream << this;
//  if (accept_) {
//    repr_stream << "(A)";
//  }
//  for (auto it = out_edges_.begin(); it != out_edges_.end(); ++it) {
//    repr_stream << "\n  " << it-> first << " " << it->second;
//  }
//  return repr_stream.str();
//}

size_t NodeHash::operator()(const std::shared_ptr<Node>& node) const
{
  size_t seed = 0;
  boost::hash_combine(seed, node->accept_);
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

bool operator==(const Node& lhs, const Node& rhs)
{
  return (lhs.get_accept() == rhs.get_accept()
          && lhs.get_in_degree() == rhs.get_in_degree()
          && lhs.get_out_degree() == rhs.get_out_degree()
          && std::equal(lhs.get_in_edges().begin(), lhs.get_in_edges().end(),
                        rhs.get_in_edges().begin(), in_edge_equal)
          && std::equal(lhs.get_out_edges().begin(), lhs.get_out_edges().end(),
                    rhs.get_out_edges().begin()));
}

bool operator!=(const Node& lhs, const Node& rhs)
{
  return !operator==(lhs, rhs);
}
