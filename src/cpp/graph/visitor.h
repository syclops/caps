//
// Created by smaptas on 23.10.17.
//

#ifndef CAPS_VISITOR_H
#define CAPS_VISITOR_H

// Include C++ standard libraries
#include <memory>

// Include other headers from this project
#include "node.h"

class GraphVisitor
{
 public:

  virtual ~GraphVisitor();

  std::shared_ptr<GraphVisitor> clone() const;

  virtual void process_node(const std::shared_ptr<Node>& node);

  virtual void process_label(const std::string& label);

  virtual bool should_proceed(const std::shared_ptr<Node>& node,
                              const std::string& label) const;

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
