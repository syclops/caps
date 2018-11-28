
// Include header.
#include "graph.h"

// Include C++ standard libraries.
#include <set>
#include <unordered_set>

// Include other headers from this project.
#include "../../common/type_debug.h"

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

LabeledGraph::LabeledGraph()
  : nodes_{}, label_counts_{},
    root_{new Node}, num_accept_{0}, num_edges_{0}, compacted_{false}
{
  nodes_.emplace(root_);
}

LabeledGraph::LabeledGraph(const LabeledGraph& orig)
  : nodes_{}, label_counts_{}, root_{nullptr},
    num_accept_{0}, num_edges_{0}, compacted_{false}
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

const LabeledGraph::LabelMap& LabeledGraph::get_label_counts() const
{
  return label_counts_;
}

void LabeledGraph::set_accept(Node* node, bool accept)
{
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
  // Only remove a node if it is in the graph.
  if (node == nullptr || !has_node(node)) {
    return;
  }

  // Create a list of edges to delete first to prevent invalidating iterators.
  std::vector<Node::HalfEdge> in_edges;
  for (const auto& in_edge: node->get_in_edges()) {
    in_edges.emplace_back(in_edge);
  }
  for (const auto& [label, parent]: in_edges){
    remove_edge(parent, label);
  }

  std::vector<Node::HalfEdge> out_labels;
  for (const auto& out_edge: node->get_out_edges()) {
    out_labels.emplace_back(out_edge);
  }
  for (const auto& [label, child]: out_labels) {
    remove_edge(node, label);
    if (child->get_in_degree() == 0) {
      remove_node(child);
    }
  }

  remove_raw_node(node);
}

void LabeledGraph::add_edge(Node* source, Node* destination,
                            const std::string& label)
{
  // If either the source or destination are not in the graph, do nothing.
  if (source == nullptr || destination == nullptr) {
    return;
  }
  if (nodes_.find(source) == nodes_.end()) {
    return;
  }
  if (nodes_.find(destination) == nodes_.end()) {
    return;
  }
  // If the outgoing label exists at the source, remove it to ensure that the
  // destination of that edge is updated correctly.
  Node* existing_child = nullptr;
  if (source->has_out_label(label)) {
    existing_child = source->follow_out_edge(label);
    remove_edge(source, label);
  }
  source->add_out_edge(label, destination);
  destination->add_in_edge(label, source);
  if (existing_child != nullptr && existing_child->get_in_degree() == 0) {
    remove_node(existing_child);
  }
  // Update the number of edges, since it has to be done manually.
  ++num_edges_;
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
    return;
  }

  // Remove the edge from both incident nodes' edge maps.
  auto dest = source->follow_out_edge(label);
  if (dest == nullptr || !has_node(dest)) {
    return;
  }

  remove_raw_edge(source, dest, label);
}

LabeledGraph::NodeHandle LabeledGraph::add_unattached_node()
{
  auto new_node = new Node;
  nodes_.emplace(new_node);
  return new_node;
}

void LabeledGraph::remove_raw_node(Node* node)
{
  if (!has_node(node)) {
    return;
  }
  if (node->get_accept()) {
    --num_accept_;
  }
  nodes_.erase(nodes_.find(node));
}

void LabeledGraph::remove_raw_edge(Node* source, Node* dest,
                                   const std::string& label)
{
  if (source == nullptr || dest == nullptr) {
    return;
  }
  if (has_node(source) && source->has_out_edge(label, dest)) {
    source->remove_out_edge(label);
  }
  if (has_node(dest) && dest->has_in_edge(label, source)) {
    dest->remove_in_edge(label, source);
  }
  --num_edges_;
  --label_counts_[label];
  if (label_counts_[label] == 0) {
    label_counts_.erase(label);
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
      if (node_table.find(lhs_edge_itr->second) == node_table.end()) {
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
