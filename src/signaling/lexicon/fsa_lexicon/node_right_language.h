/**
 * Hash and comparison functors for the right language of a node.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_NODE_RIGHT_LANGUAGE_H
#define CAPS_NODE_RIGHT_LANGUAGE_H

// Include C standard libraries.

// Include C++ standard libraries.

// Include other headers from this project.
#include "../../graph/node/node.h"

// Include headers from other projects.

/**
 * Functor to hash the right language of a node.
 */
struct NodeRightLanguageHash
{
  std::size_t operator()(const Node* node) const;
};

/**
 * Functor to determine if two nodes are equivalent.
 */
struct NodeRightLanguagePred
{
  bool operator()(const Node* lhs, const Node* rhs) const;
};


#endif //CAPS_NODE_RIGHT_LANGUAGE_H
