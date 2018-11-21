/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_TRANSITIVE_PATH_VISITOR_H
#define CAPS_TRANSITIVE_PATH_VISITOR_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>

// Include other headers from this project.
#include "../node/node.h"
#include "../visitor/visitor.h"
#include "connected_component.h"

// Include headers from other projects.

class TransitivePathVisitor: public ConstEdgeVisitor<OutEdgeTraits>
{
 public:

  // Type aliases
  using NodeHandle = const Node*;
  using Path = std::tuple<NodeHandle, NodeHandle, std::string>;

  // Disable default construction.
  TransitivePathVisitor() = delete;

  explicit TransitivePathVisitor(const ConnectedComponent& component);

  void setup() override;

  void finish() override;

  void visit_node(const Node* node) override;

  void visit_edge(const Node* source, const Node* destination,
                  const std::string& label) override;

  bool should_visit_edge(const Node* source, const Node* dest,
                         const std::string&) const override;

  const std::vector<Path>& get_result() const;

 private:

  std::vector<Path> paths_;
  std::unordered_map<NodeHandle, std::set<std::string>> transitive_paths_;
  NodeHandle source_;
  const ConnectedComponent& component_;

};

#endif //CAPS_TRANSITIVE_PATH_VISITOR_H
