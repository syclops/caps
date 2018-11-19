/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#include "transitive_path_visitor.h"

// Include C standard libraries.

// Include C++ standard libraries.
#include <iostream>

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
  std::cerr << "visit " << node << std::endl;
  if (node != nullptr && source_ == nullptr) {
    std::cerr << "set source" << std::endl;
    source_ = node;
    transitive_paths_[source_].emplace();
  }
}

void TransitivePathVisitor::visit_edge(const Node* source,
                                       const Node* destination,
                                       const std::string& label)
{
  std::cerr << source << ", " << destination << ", " << label << std::endl;
  if (source == nullptr || destination == nullptr) {
    return;
  }
  for (const auto& path_label: transitive_paths_[source]) {
    std::cerr << "add " << path_label + label << std::endl;
    transitive_paths_[destination].emplace(path_label + label);
  }
}

bool TransitivePathVisitor::should_visit_edge(const Node* source,
                                              const Node* dest,
                                              const std::string&) const
{
  auto b = source != nullptr && dest != nullptr
         && (component_.has_node(dest)
             || (component_.has_node(source)
                 && component_.has_downstream_node(dest)));
  std::cerr << "should" << (b ? "" : "n't") << " visit " << source << ", "
            << dest << std::endl;
  return b;
}

const std::vector<TransitivePathVisitor::Path>&
TransitivePathVisitor::get_result() const
{
  return paths_;
}

