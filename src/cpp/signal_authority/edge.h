/**
 * Author: smaptas
 */

#ifndef CAPS_EDGE_H
#define CAPS_EDGE_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <memory>

// Include other headers from this project.
#include "../graph/node.h"

// Include headers from other projects.

class Edge
{
 public:

  Edge(std::shared_ptr<Node> source, std::shared_ptr<Node> dest,
       std::string label);

  std::shared_ptr<Node> get_source() const;

  std::shared_ptr<Node> get_dest() const;

  std::string get_label() const;

 private:
  std::shared_ptr<Node> source_;
  std::shared_ptr<Node> dest_;
  std::string label_;
};

#endif //CAPS_EDGE_H
