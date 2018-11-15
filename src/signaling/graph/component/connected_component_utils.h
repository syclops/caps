/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_CONNECTED_COMPONENT_UTILS_H
#define CAPS_CONNECTED_COMPONENT_UTILS_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <vector>

// Include other headers from this project.
#include "connected_component.h"
#include "transitive_path_visitor.h"

// Include headers from other projects.

/**
 * Organize a set of nodes into a set of connected components.
 *
 * Given a set of nodes, form a set of connected components based on the
 * edges among the nodes in the set. The set of nodes must be from the same
 * graph. A connected component is a set of nodes in which any two nodes can
 * be reached from one another by following a series of directed edges or
 * reverse directed edges that only pass through nodes in the component.
 *
 * @param nodes - set of nodes from which to make connected components.
 * @return - the set of connected components (as ConnectedComponent instances).
 */
std::vector<ConnectedComponent> make_connected_components(
  ConnectedComponent::NodeSet nodes);

std::vector<TransitivePathVisitor::Path> get_transitive_paths(
  const ConnectedComponent& component);

#endif //CAPS_CONNECTED_COMPONENT_UTILS_H
