//
// Created by smaptas on 23.10.17.
//

#ifndef CAPS_VISITOR_H
#define CAPS_VISITOR_H

// Include C++ standard libraries
#include <memory>

// Include other headers from this project
#include "../node/node.h"

template <typename Iterator>
class Visitor
{
 public:

  virtual ~Visitor() = default;

  virtual void setup()
  {
    // Nothing to do here.
  }

  virtual void finish()
  {
    // Nothing to do here.
  }

  virtual void visit_node(const Node*)
  {
    // Nothing to do here.
  }

  virtual void visit_edge(const Node*, const Node*, const std::string&)
  {
    // Nothing to do here.
  }

  virtual bool should_visit_node(const Node*) const
  {
    return true;
  }

  virtual bool should_visit_edge(const Node*, const Node*,
                                 const std::string&) const
  {
    return true;
  }

  virtual Iterator begin(const Node* node) const = 0;

  virtual Iterator end(const Node* end) const = 0;
};

class InEdgeTraits
{
 public:

  using edge_set = Node::InEdgeSet;

  static const edge_set& get_edges(const Node* node)
  {
    return node->get_in_edges();
  }
};

class OutEdgeTraits
{
 public:

  using edge_set = Node::OutEdgeSet;

  static const edge_set& get_edges(const Node* node)
  {
    return node->get_out_edges();
  }
};

template <typename EdgeTraits>
using ConstIterator = typename EdgeTraits::edge_set::const_iterator;

template <typename EdgeTraits>
class ConstEdgeVisitor: public Visitor<ConstIterator<EdgeTraits>>
{
 public:

  using iterator = ConstIterator<EdgeTraits>;

  iterator begin(const Node* node) const
  {
    return EdgeTraits::get_edges(node).cbegin();
  }

  iterator end(const Node* node) const
  {
    return EdgeTraits::get_edges(node).cend();
  }
};

template <typename EdgeTraits>
using ConstReverseIterator =
  typename EdgeTraits::edge_set::const_reverse_iterator;

template <typename EdgeTraits>
class ConstReverseEdgeVisitor: public Visitor<ConstReverseIterator<EdgeTraits>>
{
 public:

  using iterator = ConstReverseIterator<EdgeTraits>;

  iterator begin(const Node* node) const
  {
    return EdgeTraits::get_edges(node).crbegin();
  }

  iterator end(const Node* node) const
  {
    return EdgeTraits::get_edges(node).crend();
  }
};

class GraphVisitor
{
 public:

  virtual ~GraphVisitor();

  std::shared_ptr<GraphVisitor> clone() const;

  virtual void process_node(const Node&);

  virtual void process_label(const std::string&);

  virtual bool should_proceed(const Node&, const Node&,
                              const std::string&) const;

 private:

  virtual GraphVisitor* clone_raw() const = 0;

};

template <typename ConcreteVisitor, typename AbstractVisitor>
class CloneableVisitor: public AbstractVisitor
{
 public:
  std::shared_ptr<ConcreteVisitor> clone() const
  {
    return std::shared_ptr<ConcreteVisitor>(
      static_cast<ConcreteVisitor*>(this->clone_raw()));
  }

 private:
  virtual CloneableVisitor* clone_raw() const
  {
    return new ConcreteVisitor(*static_cast<const ConcreteVisitor*>(this));
  }
};

#endif //CAPS_VISITOR_H
