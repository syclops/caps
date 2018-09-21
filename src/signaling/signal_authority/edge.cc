/**
 * Author: smaptas
 */

// Include header file.
#include "edge.h"

// Include C standard libraries.

// Include C++ standard libraries.
#include <memory>
#include <utility>

// Include other headers from this project.
#include "../graph/node.h"

// Include header from other projects.

Edge::Edge(std::shared_ptr<Node> source, std::shared_ptr<Node> dest,
           std::string label)
  : source_{source}, dest_{dest}, label_{std::move(label)}
{
  // Nothing to do here.
}

std::shared_ptr<Node> Edge::get_source() const
{
  return source_;
}

std::shared_ptr<Node> Edge::get_dest() const
{
  return dest_;
}

std::string Edge::get_label() const
{
  return label_;
}
