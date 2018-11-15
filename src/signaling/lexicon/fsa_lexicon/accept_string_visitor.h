/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_ACCEPT_STRING_VISITOR_H
#define CAPS_ACCEPT_STRING_VISITOR_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <set>
#include <string>
#include <unordered_map>

// Include other headers from this project.
#include "../../graph/node/node.h"
#include "../../graph/visitor/visitor.h"

// Include headers from other projects.

class AcceptStringVisitor: public ConstReverseEdgeVisitor<OutEdgeTraits>
{

 public:

  AcceptStringVisitor();

  void setup() override;

  void finish() override;

  void visit_edge(const Node* source, const Node* dest,
                  const std::string& label) override;

  std::set<std::string> get_result() const;

 private:

  std::set<std::string> accept_strings_;
  std::unordered_map<const Node*, std::set<std::string>> transitive_paths_;
};

#endif //CAPS_ACCEPT_STRING_VISITOR_H
