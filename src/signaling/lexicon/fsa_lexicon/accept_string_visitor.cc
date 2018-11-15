/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

// Include header file.
#include "accept_string_visitor.h"

// Include C standard libraries.

// Include C++ standard libraries.
#include <set>
#include <string>
#include <unordered_map>

// Include other headers from this project.
#include "../../graph/node/node.h"

// Include headers from other projects.

AcceptStringVisitor::AcceptStringVisitor()
{
  // Nothing to do here.
}

void AcceptStringVisitor::visit_edge(const Node* source, const Node* dest,
                                     const std::string& label)
{
  for (const auto& partial_string: transitive_paths_[source]) {
    transitive_paths_[dest].insert(partial_string + label);
  }
}

std::set<std::string> AcceptStringVisitor::get_result() const
{
  return accept_strings_;
}

void AcceptStringVisitor::setup()
{
  accept_strings_.clear();
  transitive_paths_.clear();
}

void AcceptStringVisitor::finish()
{
  for (const auto& pair: transitive_paths_) {
    auto&& [node, strings] = pair;
    if (node->get_accept()) {
      accept_strings_.insert(strings.begin(), strings.end());
    }
  }
}
