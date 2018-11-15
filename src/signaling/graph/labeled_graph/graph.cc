
// Include header.
#include "graph.h"

// Include C++ standard libraries.
#include <unordered_set>

// Include other headers from this project.
#include "../../common/contains.h"
#include "../../common/hash.h"
#include "../../common/type_debug.h"

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

LabeledGraph::LabeledGraph()
  : root_{new Node}, num_accept_{0}, num_edges_{0}, compacted_{false}, nodes_{},
    source_counts_{}, dest_counts_{}, label_counts_{}
{
  nodes_.emplace(root_);
}

LabeledGraph::LabeledGraph(const LabeledGraph& orig)
  : root_{nullptr}, num_accept_{0}, num_edges_{0}, compacted_{false}, nodes_{},
    source_counts_{}, dest_counts_{}, label_counts_{}
{
  // Create a translation table of node pointers from the original graph.
  std::unordered_map<Node*, Node*> node_table;
  for (const auto& orig_node: orig.get_nodes()) {
    auto orig_node_ptr = orig_node.get();
    auto copy_node = add_unattached_node();
    if (orig_node_ptr == orig.root_) {
      root_ = copy_node;
    }
    node_table.emplace(orig_node.get(), copy_node);
  }

  // Add the edges to the graph.
  for (const auto& orig_node: orig.get_nodes()) {
    auto copy_node = node_table.at(orig_node.get());
    copy_node->set_accept(orig_node->get_accept());
    for (auto out_edge: orig_node->get_out_edges()) {
      const auto& [label, downstream_node] = out_edge;
      auto downstream_copy = node_table.at(downstream_node);
      add_edge(copy_node, downstream_copy, label);
    }
  }
}

Node* LabeledGraph::get_root() const
{
  return root_;
}

size_t LabeledGraph::get_num_accept() const
{
  return num_accept_;
}

size_t LabeledGraph::get_num_nodes() const
{
  return nodes_.size();
}

size_t LabeledGraph::get_num_edges() const
{
  return num_edges_;
}

bool LabeledGraph::get_compacted() const
{
  return compacted_;
}

const LabeledGraph::NodeSet& LabeledGraph::get_nodes() const
{
  return nodes_;
}

const LabeledGraph::NodeMap& LabeledGraph::get_source_counts() const
{
  return source_counts_;
}

const LabeledGraph::NodeMap& LabeledGraph::get_dest_counts() const
{
  return dest_counts_;
}

const LabeledGraph::LabelMap& LabeledGraph::get_label_counts() const
{
  return label_counts_;
}

void LabeledGraph::set_accept(Node* node, bool accept)
{
  std::cerr << "set_accept: " << node << std::endl;
  if (node == nullptr || nodes_.find(node) == nodes_.end()) {
    return;
  }
  if (!node->get_accept() && accept) {
    ++num_accept_;
  } else if (node->get_accept() && !accept) {
    --num_accept_;
  }
  node->set_accept(accept);
}

LabeledGraph::NodeHandle LabeledGraph::add_node(Node* source,
                                                const std::string& label)
{
  return add_edge(source, label);
}

void LabeledGraph::remove_node(Node* node)
{
  std::cerr << "remove_node: " << node << std::endl;
  // Only remove a node if it is in the graph.
  if (node == nullptr || !has_node(node)) {
    std::cerr << "  remove_node: null or non-member" << std::endl;
    return;
  }

  // Create a list of edges to delete first to prevent invalidating iterators.
  std::vector<Node::HalfEdge> in_edges;
  for (const auto& in_edge: node->get_in_edges()) {
    in_edges.emplace_back(in_edge);
  }
  for (const auto& [label, parent]: in_edges){
    std::cerr << "  remove_node: remove edge " << label << " from " << parent
              << std::endl;
    remove_edge(parent, label);
  }

  std::vector<Node::HalfEdge> out_labels;
  for (const auto& out_edge: node->get_out_edges()) {
    out_labels.emplace_back(out_edge);
  }
  for (const auto& [label, child]: out_labels) {
    std::cerr << "  remove_node: remove edge " << label << " to " << child
              << std::endl;
    remove_edge(node, label);
    if (child->get_in_degree() == 0) {
      std::cerr << "  remove_node: recursing" << std::endl;
      remove_node(child);
    }
  }

  remove_raw_node(node);
}

void LabeledGraph::add_edge(Node* source, Node* destination,
                            const std::string& label)
{
  std::cerr << "add_edge: " << source << ", " << destination << ", " << label
            << std::endl;
  // If either the source or destination are not in the graph, do nothing.
  if (source == nullptr || destination == nullptr) {
    std::cerr << "Can't create edge with null pointer" << std::endl;
    return;
  }
  if (nodes_.find(source) == nodes_.end()) {
    std::cerr << "Source " << source << " not in node set" << std::endl;
    return;
  }
  if (nodes_.find(destination) == nodes_.end()) {
    std::cerr << "Dest " << destination << " not in node set" << std::endl;
    return;
  }
  // If the outgoing label exists at the source, remove it to ensure that the
  // destination of that edge is updated correctly.
  Node* existing_child = nullptr;
  if (source->has_out_label(label)) {
    std::cerr << "Removing existing label " << label << " from " << source
              << std::endl;
    existing_child = source->follow_out_edge(label);
    remove_edge(source, label);
  }
  source->add_out_edge(label, destination);
  destination->add_in_edge(label, source);
  if (existing_child != nullptr && existing_child->get_in_degree() == 0) {
    std::cerr << "add_edge: remove displaced child node" << std::endl;
    remove_node(existing_child);
  }
  // Update the number of edges, since it has to be done manually.
  ++num_edges_;
  ++source_counts_[source];
  ++dest_counts_[destination];
  ++label_counts_[label];
}

LabeledGraph::NodeHandle LabeledGraph::add_edge(Node* source,
                                                const std::string& label)
{
  auto new_node = add_unattached_node();
  add_edge(source, new_node, label);
  return new_node;
}

void LabeledGraph::remove_edge(Node* source, const std::string& label)
{
  // Do nothing if the source is null or does not have the label.
  if (source == nullptr || !has_node(source)) {
    std::cerr << "  remove_edge: null or non-member source" << std::endl;
    return;
  }

  // Remove the edge from both incident nodes' edge maps.
  auto dest = source->follow_out_edge(label);
  if (dest == nullptr || !has_node(dest)) {
    std::cerr << "  remove_edge: null or non-member dest" << std::endl;
    return;
  }

  std::cerr << "remove_edge: " << source << ", " << dest << ", " << label
            << std::endl;

  remove_raw_edge(source, dest, label);
}

LabeledGraph::NodeHandle LabeledGraph::add_unattached_node()
{
  auto new_node = new Node;
  std::cerr << "add_unattached_node: " << new_node << std::endl;
  nodes_.emplace(new_node);
  return new_node;
}

void LabeledGraph::remove_raw_node(Node* node)
{
  if (!has_node(node)) {
    std::cerr << "remove_raw_node: non-member node" << std::endl;
    return;
  }
  if (node->get_accept()) {
    --num_accept_;
  }
  source_counts_.erase(node);
  dest_counts_.erase(node);
  nodes_.erase(nodes_.find(node));
}

void LabeledGraph::remove_raw_edge(Node* source, Node* dest,
                                   const std::string& label)
{
  if (source == nullptr || dest == nullptr) {
    std::cerr << "remove_raw_edge: incident node is null" << std::endl;
    return;
  }
  if (!has_node(source)) {
    std::cerr << "remove_raw_edge: non-member source node" << std::endl;
  }
  if (!has_node(dest)) {
    std::cerr << "remove_raw_edge: non-member dest node" << std::endl;
  }
  if(!source->has_out_edge(label, dest)) {
    std::cerr << "remove_raw_edge: out-edge not found" << std::endl;
  }
  if(!dest->has_in_edge(label, source)) {
    std::cerr << "remove_raw_edge: in-edge not found" << std::endl;
  }
  if (has_node(source) && source->has_out_edge(label, dest)) {
    remove_raw_out_edge(source, label);
  }
  if (has_node(dest) && dest->has_in_edge(label, source)) {
    remove_raw_in_edge(dest, source, label);
  }
  --num_edges_;
  --label_counts_[label];
  if (label_counts_[label] == 0) {
    label_counts_.erase(label);
  }
}

void LabeledGraph::remove_raw_in_edge(Node* node, Node* parent,
                                      const std::string& label)
{
  std::cerr << "remove_raw_in_edge: " << parent << ", " << node << ", " << label
            << std::endl;
  node->remove_in_edge(label, parent);
  --dest_counts_[node];
  if (dest_counts_[node] == 0) {
    dest_counts_.erase(node);
  }
}

void LabeledGraph::remove_raw_out_edge(Node* node, const std::string& label)
{
  std::cerr << "remove_raw_out_edge: " << node << ", " << label
            << std::endl;
  node->remove_out_edge(label);
  --source_counts_[node];
  if (source_counts_[node] == 0) {
    source_counts_.erase(node);
  }
}

bool LabeledGraph::has_node(Node* node) const
{
  return nodes_.find(node) != nodes_.end();
}

bool operator==(const LabeledGraph& lhs, const LabeledGraph& rhs)
{
  // Immediately reject if the high-level graph properties don't match. (We
  // don't compare the counts of sources or destinations, as that requires
  // creating a graph isomorphism, or the labels, as that requires comparing
  // string references.
  if (lhs.get_num_nodes() != rhs.get_num_nodes()
      || lhs.get_num_accept() != rhs.get_num_accept()
      || lhs.get_num_edges() != rhs.get_num_edges()) {
    return false;
  }

  // Declare type aliases.
  using NodePtrType = const Node*;
  using PairType = std::pair<NodePtrType, NodePtrType>;

  // Perform a BFS in an attempt to create a graph isomorphism.
  std::unordered_map<NodePtrType, NodePtrType> node_table;
  auto left_root = lhs.get_root();
  auto right_root = rhs.get_root();
  node_table.emplace(left_root, right_root);
  std::queue<PairType> queue;
  queue.emplace(left_root, right_root);
  while (!queue.empty()) {
    // Get a pair of node pointers from the queue.
    const auto& [first, second] = queue.front();
    queue.pop();

    // Reject if nodes do not have the same accept status or if their
    // out-degrees or in-degrees differ
    if (first->get_accept() != second->get_accept()
        || first->get_in_degree() != second->get_in_degree()
        || first->get_out_degree() != second->get_out_degree()) {
      return false;
    }

    for (auto lhs_edge_itr = first->get_out_edges().begin(),
              rhs_edge_itr = second->get_out_edges().begin();
         lhs_edge_itr != first->get_out_edges().end()
           && rhs_edge_itr != second->get_out_edges().end();
         ++lhs_edge_itr, ++rhs_edge_itr) {

      // Add a pair of newly-visited nodes to the isomorphism table
      if (!contains(node_table, lhs_edge_itr->second)) {
        node_table.emplace(lhs_edge_itr->second, rhs_edge_itr->second);
        queue.emplace(lhs_edge_itr->second, rhs_edge_itr->second);
      }

      // Reject if they have mismatched edge labels (traversed in
      // alphabetical order) or if their destinations do not match
      if (lhs_edge_itr->first != rhs_edge_itr->first
          || node_table.at(lhs_edge_itr->second) != rhs_edge_itr->second) {
        return false;
      }
    }
  }

  return true;
}

bool operator!=(const LabeledGraph& lhs, const LabeledGraph& rhs)
{
  return !operator==(lhs, rhs);
}
