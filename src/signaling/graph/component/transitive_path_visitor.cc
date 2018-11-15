/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#include "transitive_path_visitor.h"

// Include C standard libraries.

// Include C++ standard libraries.

// Include other headers from this project.

// Include headers from other projects.

TransitivePathVisitor::TransitivePathVisitor(
  const ConnectedComponent& component)
  : source_{nullptr}, component_{component}
{
  // Nothing to do here.
}

void TransitivePathVisitor::setup()
{
  source_ = nullptr;
  transitive_paths_.clear();
}

void TransitivePathVisitor::finish()
{
  for (const auto& downstream_node: component_.downstream_nodes()) {
    for (const auto& path_label: transitive_paths_[downstream_node]) {
      paths_.emplace_back(source_, downstream_node, path_label);
    }
  }
}

void TransitivePathVisitor::visit_node(const Node* node)
{
  if (node != nullptr && source_ == nullptr) {
    source_ = node;
  }
}

void TransitivePathVisitor::visit_edge(const Node* source,
                                       const Node* destination,
                                       const std::string& label)
{
  if (source == nullptr || destination == nullptr) {
    return;
  }
  for (const auto& path_label: transitive_paths_[source]) {
    transitive_paths_[destination].emplace(path_label + label);
  }
}

bool TransitivePathVisitor::should_visit_edge(const Node* source,
                                              const Node* dest,
                                              const std::string&) const
{
  return source != nullptr && dest != nullptr
         && (component_.has_node(dest)
             || (component_.has_node(source)
                 && component_.has_downstream_node(dest)));
}

const std::vector<TransitivePathVisitor::Path>&
TransitivePathVisitor::get_result() const
{
  return paths_;
}

