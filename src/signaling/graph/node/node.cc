/**
 *
 */

// Include header file.
#include "node.h"

// Include C++ standard libraries.
#include <iostream>
#include <optional>
#include <string>

// Include other headers from this project.

// Include headers from other projects.

Node::Node()
  : in_edges_{}, out_edges_{}, accept_{false}
{
  // Nothing to do here.
}

bool Node::get_accept() const noexcept
{
  return accept_;
}

size_t Node::get_in_degree() const noexcept
{
  return in_edges_.size();
}

size_t Node::get_out_degree() const noexcept
{
  return out_edges_.size();
}

bool Node::has_in_edge(const Label& label, Node* source) const
{
  return in_edges_.find(std::make_pair(label, source)) != in_edges_.end();
}

bool Node::has_out_edge(const Label& label, const Node* target) const
{
  return follow_out_edge(label) == target;
}

bool Node::has_out_label(const Label& label) const
{
  return out_edges_.find(label) != out_edges_.end();
}

void Node::set_accept(bool accept) noexcept
{
  accept_ = accept;
}

void Node::add_in_edge(const Label& label, Node* source)
{
  in_edges_.emplace(label, source);
}

void Node::remove_in_edge(const Label& label, Node* source)
{
  in_edges_.erase(std::make_pair(label, source));
}

void Node::add_out_edge(const Label& label, Node* target)
{
  out_edges_.emplace(label, target);
}

void Node::remove_out_edge(const Label& label)
{
  out_edges_.erase(label);
}

Node* Node::follow_out_edge(const Label& label) const
{
  return has_out_label(label) ? out_edges_.at(label) : nullptr;
}

const std::set<Node::HalfEdge>& Node::get_in_edges() const
{
  return in_edges_;
}

const std::map<Node::Label, Node*>& Node::get_out_edges() const
{
  return out_edges_;
}

ReverseIterable<Node::InEdgeSet> Node::get_reverse_in_edges() const
{
  return {in_edges_};
}

ReverseIterable<Node::OutEdgeSet> Node::get_reverse_out_edges() const
{
  return {out_edges_};
}

bool operator==(const Node& lhs, const Node& rhs)
{
  return (lhs.get_accept() == rhs.get_accept()
          && lhs.get_in_degree() == rhs.get_in_degree()
          && lhs.get_out_degree() == rhs.get_out_degree()
          && std::equal(lhs.get_in_edges().begin(), lhs.get_in_edges().end(),
                        rhs.get_in_edges().begin())
          && std::equal(lhs.get_out_edges().begin(), lhs.get_out_edges().end(),
                        rhs.get_out_edges().begin()));
}

bool operator!=(const Node& lhs, const Node& rhs)
{
  return !operator==(lhs, rhs);
}

