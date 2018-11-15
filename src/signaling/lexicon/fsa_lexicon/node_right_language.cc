/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

// Include header file.
#include "node_right_language.h"

// Include C standard libraries.

// Include C++ standard libraries.
// TODO: remove when done debugging.
#include <iostream>

// Include other headers from this project.
#include "../../graph/node/node.h"

// Include headers from other projects.
#include <boost/functional/hash.hpp>

std::size_t NodeRightLanguageHash::operator()(const Node* node) const
{
  size_t seed = 0;
  boost::hash_combine(seed, node->get_accept());
  for (const auto& [label, child]: node->get_out_edges()) {
    boost::hash_combine(seed, label);
    boost::hash_combine(seed, child);
  }
  return seed;
}

bool NodeRightLanguagePred::operator()(const Node* lhs, const Node* rhs) const
{
//  std::cerr << "lhs (" << lhs << "): accept " << lhs->get_accept()
//            << ", out-degree " << lhs->get_out_degree() << std::endl;
//  for (const auto& [label, parent]: lhs->get_out_edges()) {
//    std::cerr << "  " << label << ": " << parent << std::endl;
//  }
//  std::cerr << "rhs (" << rhs << "): accept " << rhs->get_accept()
//            << ", out-degree " << rhs->get_out_degree() << std::endl;
//  for (const auto& [label, parent]: rhs->get_out_edges()) {
//    std::cerr << "  " << label << ": " << parent << std::endl;
//  }
  if (lhs == nullptr || rhs == nullptr) {
    return lhs == rhs;
  }
  if (lhs->get_accept() != rhs->get_accept()
      || lhs->get_out_degree() != rhs->get_out_degree()) {
    return false;
  }
  if (lhs->get_out_degree() == 0) {
    return true;
  }
  return std::equal(lhs->get_out_edges().begin(), lhs->get_out_edges().end(),
                    rhs->get_out_edges().begin());
}

