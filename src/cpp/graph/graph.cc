
#include "graphbase.h"
#include <iostream>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "node.h"

void GraphVisitor::process_node(const std::shared_ptr<Node>& node)
{
  // Nothing to do here.
}

void GraphVisitor::process_label(const std::string& label)
{
  // Nothing to do here.
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////////////////////

GraphBase::GraphBase()
  : root_{new Node}, num_accept_{0}, num_edges_{0}, compacted_{false}, nodes_{}
{
  nodes_.insert(root_);
}

GraphBase::GraphBase(const GraphBase& orig)
  : root_{new Node}, num_accept_{0}, num_edges_{0}, compacted_{false}, nodes_{}
{
  // Create a translation table of node pointers from the original graph.
  std::unordered_map<std::shared_ptr<Node>, std::shared_ptr<Node>> node_table;
  for (auto orig_node_ptr: orig.get_nodes()) {
    node_table.emplace(orig_node_ptr, add_unattached_node());
  }
  // Add the edges to the graph.
  for (auto orig_node_ptr: orig.get_nodes()) {
    node_table.at(orig_node_ptr)->set_accept(orig_node_ptr->get_accept());
    for (auto out_edge_itr: orig_node_ptr->get_out_edges()) {
      add_edge(orig_node_ptr, out_edge_itr.second, out_edge_itr.first);
    }
  }
}

int GraphBase::get_num_nodes() const
{
  return static_cast<int>(nodes_.size());
}

int GraphBase::get_num_edges() const
{
  return num_edges_;
}

int GraphBase::get_num_accept() const
{
  return num_accept_;
}

bool GraphBase::get_compacted() const
{
  return compacted_;
}

std::shared_ptr<Node> GraphBase::get_root() const
{
  return root_;
}

std::shared_ptr<Node> GraphBase::add_node(std::shared_ptr<Node> source,
                                          std::string label)
{
  return add_edge(source, label);
}

void GraphBase::remove_node(std::shared_ptr<Node> node)
{
  // Because removing in-edges and out-edges as we iterate through them would
  // invalidate the relevant iterators, we copy the relevant information into a
  // vector before removing the edges.
  std::vector<std::pair<std::weak_ptr<Node>, std::string>> in_edges;
  for (auto edge_itr: node->get_in_edges()) {
    in_edges.emplace_back(edge_itr.second, edge_itr.first);
  }
  for (auto in_edge_itr: in_edges){
    remove_edge(in_edge_itr.first.lock(), in_edge_itr.second);
  }
  std::vector<std::string> out_labels;
  for (auto edge_itr: node->get_out_edges()) {
    out_labels.push_back(edge_itr.first);
  }
  for (const auto& label: out_labels) {
    remove_edge(node, label);
  }
  // Once the node's incident edges have been safely deleted, remove the node
  // from the nodes list.
  nodes_.erase(node);
}

void GraphBase::add_edge(std::shared_ptr<Node> source,
                         std::shared_ptr<Node> destination, std::string label)
{
  source->add_out_edge(label, destination);
  destination->add_in_edge(label, std::weak_ptr<Node>(source));
  // Update the number of edges, since it has to be done manually.
  ++num_edges_;
}

std::shared_ptr<Node> GraphBase::add_edge(std::shared_ptr<Node> source,
                                          std::string label)
{
  auto new_node = add_unattached_node();
  add_edge(source, new_node, label);
  return new_node;
}

void GraphBase::remove_edge(std::shared_ptr<Node> source, std::string label)
{
  // Remove the edge from both incident nodes' edge maps.
  auto dest_ptr = source->follow_out_edge(label);
  dest_ptr->remove_in_edge(label, std::weak_ptr<Node>{source});
  source->remove_out_edge(label);
  // If removing this edge has created an orphaned node, remove the node.
  if (dest_ptr->get_in_degree() == 0) {
    remove_node(dest_ptr);
  }
  // Update the number of edges, since it has to be done manually.
  --num_edges_;
}

GraphBase::NodeIterable::NodeIterable(const nodes_t& nodes)
  : nodes_{nodes}
{
  // Nothing to do here.
}

GraphBase::NodeIterable::nodes_t::const_iterator
GraphBase::NodeIterable::begin() const
{
  return nodes_.cbegin();
}

GraphBase::NodeIterable::nodes_t::const_iterator
GraphBase::NodeIterable::end() const
{
  return nodes_.cend();
}

GraphBase::NodeIterable GraphBase::get_nodes() const
{
  return NodeIterable{nodes_};
}

void GraphBase::visit_dfs(GraphVisitor& visitor) const
{
  std::stack<std::pair<GraphVisitor, std::shared_ptr<Node>>> stack;
  stack.emplace(std::make_pair(visitor, root_));
  while (!stack.empty()) {
    auto pair = stack.top();
    stack.pop();
    GraphVisitor node_copy{visitor};
    node_copy.process_node(pair.second);
    for (const auto &out_edge_itr: pair.second->get_reverse_out_edges()) {
      GraphVisitor label_copy{node_copy};
      label_copy.process_label(out_edge_itr.first);
      stack.emplace(std::make_pair(label_copy, out_edge_itr.second));
    }
  }
}

//GraphBase::GraphBase(const GraphBase& orig)
//  : num_strings_(orig.num_strings_), num_bytes_(orig.num_bytes_),
//    num_nodes_(orig.num_nodes_), num_accept_(orig.num_accept_),
//    num_edges_(orig.num_edges_), compacted_(orig.compacted_)
//{
//  // TODO: copy num_accept_ properly
//  std::unordered_map<Node*, Node* > ptr_map;
//  for (auto it = orig.nodes_.begin(); it != orig.nodes_.end(); ++it) {
//    ptr_map[*it] = add_node();
//    nodes_.insert(ptr_map[*it]);
//  }
//  for (auto it = orig.nodes_.begin(); it != orig.nodes_.end(); ++it) {
//    Node* orig_node = *it;
//    Node* node = ptr_map[orig_node];
//    node->copy_properties(orig_node);
//    for (auto child_it = orig_node->get_edges_cbegin();
//         child_it != orig_node->get_edges_cend(); ++child_it) {
//      add_edge(node, ptr_map[child_it->second], child_it->first);
//    }
//  }
//}
//
//GraphBase::GraphBase(GraphBase&& orig) noexcept
//  : GraphBase()
//{
//  swap(*this, orig);
//}
//
//GraphBase::~GraphBase()
//{
//  for (Node* node_ptr: nodes_) {
//    delete node_ptr;
//  }
//}
//
//void swap(GraphBase& first, GraphBase& second)
//{
//  using std::swap;
//  swap(first.root_, second.root_);
//  swap(first.num_strings_, second.num_strings_);
//  swap(first.num_bytes_, second.num_bytes_);
//  swap(first.num_nodes_, second.num_nodes_);
//  swap(first.num_accept_, second.num_accept_);
//  swap(first.num_edges_, second.num_edges_);
//  swap(first.compacted_, second.compacted_);
//  swap(first.nodes_, second.nodes_);
//}
//
//void GraphBase::read_file(std::istream& in_stream)
//{
//  std::string line;
//  while (std::getline(in_stream, line)) {
//    add_string(line);
//  }
//}
//
//std::string GraphBase::dump_dictionary() const
//{
//  std::ostringstream out_stream;
//  dump_dictionary(out_stream);
//  return out_stream.str();
//}
//
//std::string GraphBase::graph_stats() const
//{
//    std::ostringstream out_stream;
//    graph_stats(out_stream);
//    return out_stream.str();
//  }
//
//void GraphBase::graph_stats(std::ostream& out_stream) const
//{
//  out_stream << num_strings_ << " strings (" << num_bytes_ << " bytes) read"
//             << std::endl;
//  out_stream << num_nodes_ << " nodes, " << num_edges_ << " edges" << std::endl;
//}
//
//std::string GraphBase::dump_dot() const
//{
//  std::ostringstream out_stream;
//  dump_dot(out_stream);
//  return out_stream.str();
//}
//
//void GraphBase::dump_dot(std::ostream& out_stream) const
//{
//  out_stream << "digraph g {" << std::endl;
//  std::unordered_map<Node*, unsigned int> vertex_num;
//  for (Node* node: nodes_) {
//    vertex_num[node] = vertex_num.size();
//    if (node->get_accept()) {
//      out_stream << "    " << vertex_num[node] << "[label=\"1\"];" << std::endl;
//    }
//  }
//  for (Node* node: nodes_) {
//    for (auto it: node->get_out_edges()) {
//      out_stream << "    " << vertex_num[node] << " -> "
//                 << vertex_num[it.second] << " [label=\"" << it.first
//                 << "\"];" << std::endl;
//    }
//  }
//  out_stream << '}';
//}
//
//void GraphBase::dump_txt(std::ostream& out_stream) const
//{
//  out_stream << num_nodes_ << " " << num_accept_ << " " << num_edges_
//             << std::endl;
//  std::unordered_map<Node*, size_t> vertex_num;
//  std::queue<Node*> node_queue;
//  node_queue.push(root_);
//  vertex_num[root_] = 0;
//  while (!node_queue.empty()) {
//    Node* current_node = node_queue.front();
//    node_queue.pop();
//    if (current_node->get_accept()) {
//      out_stream << vertex_num[current_node] << std::endl;
//    }
//    for (Node* target: current_node->get_out_targets()) {
//      if (vertex_num.find(target) == vertex_num.end()) {
//        vertex_num[target] = vertex_num.size();
//        node_queue.push(target);
//      }
//    }
//  }
//  for (Node* node: nodes_) {
//    for (auto it: node->get_out_edges()) {
//      out_stream << vertex_num[node] << " " << vertex_num[it.second] << " "
//                 << it.first << std::endl;
//    }
//  }
//}
//
//void GraphBase::load_txt(std::istream& in_stream)
//{
//  size_t nodes, accept, edges;
//  std::unordered_map<size_t, Node*> num_to_node;
//  std::string line;
//  std::getline(in_stream, line);
//  std::stringstream(line) >> nodes >> accept >> edges;
//  for (size_t i = 0; i < nodes; ++i) {
//    num_to_node[i] = add_node();
//  }
//  remove_node(root_);
//  root_ = num_to_node[0];
//  size_t node_num;
//  for (size_t i = 0; i < accept; ++i) {
//    std::getline(in_stream, line);
//    std::stringstream(line) >> node_num;
//    set_node_accept(num_to_node[node_num]);
//  }
//  size_t target;
//  std::string label;
//  for (size_t i = 0; i < edges; ++i) {
//    std::getline(in_stream, line);
//    std::stringstream(line) >> node_num >> target >> label;
//    add_edge(num_to_node[node_num], num_to_node[target], label);
//  }
//}
//
//std::string GraphBase::dump_target_counts() const
//{
//  std::ostringstream out_stream;
//  dump_target_counts(out_stream);
//  return out_stream.str();
//}
//
//void GraphBase::dump_target_counts(std::ostream& out_stream) const
//{
//  std::unordered_map<Node*, size_t> counts = get_target_counts();
//  for (auto it: counts) {
//    out_stream << it.first << " " << it.second << std::endl;
//  }
//}
//
//bool GraphBase::in_set(const std::string str) const
//{
//  return false;
//}
//
//std::string GraphBase::dump_bin() const
//{
//  std::ostringstream out_stream;
//  dump_bin(out_stream);
//  return out_stream.str();
//}
//
//void GraphBase::compact_edges()
//{
//  std::set<Node*> visited;
//  std::queue<Node*> node_queue;
//  node_queue.push(root_);
//  while (!node_queue.empty()) {
//    Node* node = node_queue.front();
//    node_queue.pop();
//    for (const std::string& label: node->get_out_labels()) {
//      compact_edge(node, label);
//    }
//    for (Node* child: node->get_out_targets()) {
//      if (visited.find(child) == visited.end()) {
//        visited.insert(child);
//        node_queue.push(child);
//      }
//    }
//  }
//  compacted_ = true;
//}
//
//void GraphBase::uncompact_edges()
//{
//  //TODO
//  compacted_ = false;
//}
//
//size_t GraphBase::get_num_strings() const
//{
//  return num_strings_;
//}
//
//size_t GraphBase::get_num_bytes() const
//{
//  return num_bytes_;
//}
//
//size_t GraphBase::get_num_nodes() const
//{
//  return num_nodes_;
//}
//
//size_t GraphBase::get_num_edges() const
//{
//  return num_edges_;
//}
//
//std::unordered_map<std::string, size_t> GraphBase::get_label_counts() const
//{
//  std::unordered_map<std::string, size_t> label_counts;
//  for (Node* node: nodes_) {
//    for (const std::string& label: node->get_out_labels()) {
//      if (label_counts.find(label) == label_counts.end()) {
//        label_counts[label] = 0;
//      }
//      ++label_counts[label];
//    }
//  }
//  return label_counts;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////////////
//
//Node* GraphBase::add_node()
//{
//  Node* new_node = new Node;
//  nodes_.insert(new_node);
//  ++num_nodes_;
//  return new_node;
//}
//
//void GraphBase::remove_node(Node* node)
//{
//  if (nodes_.find(node) == nodes_.end()) {
//    return;
//  }
//  std::queue<Node* > deletion_queue;
//  deletion_queue.push(node);
//  while (!deletion_queue.empty()) {
//    Node* current_node = deletion_queue.front();
//    deletion_queue.pop();
//    std::vector<Node*> targets = current_node->get_out_targets();
//    for (Node* n: targets) {
//      n->remove_in_edge();
//      --num_edges_;
//      if (nodes_.find(n) != nodes_.end() && n->get_in_degree() == 0) {
//        deletion_queue.push(n);
//      }
//    }
//    nodes_.erase(current_node);
//    if (current_node->get_accept()) {
//      --num_accept_;
//    }
//    --num_nodes_;
//    delete current_node;
//  }
//}
//
//void GraphBase::add_edge(Node* source, Node* target, const std::string label)
//{
//  if (source->follow_edge(label)) {
//    remove_edge(source, label);
//  }
//  source->add_out_edge(label, target);
//  target->add_in_edge();
//  ++num_edges_;
//}
//
//void GraphBase::remove_edge(Node* source, const std::string label)
//{
//  Node* target = source->follow_edge(label);
//  if (!target) {
//    return;
//  }
//  target->remove_in_edge();
//  if (target->get_in_degree() == 0) {
//    remove_node(target);
//  }
//  source->remove_out_edge(label);
//  --num_edges_;
//}
//
//void GraphBase::set_node_accept(Node* node)
//{
//  if (node->get_accept()) {
//    return;
//  }
//  node->set_accept();
//  ++num_accept_;
//}
//
//void GraphBase::compact_edge(Node* node, const std::string label)
//{
//  std::string current_label(label);
//  Node* current_target = node->follow_edge(label);
//  while (current_target->get_in_degree() == 1
//         && current_target->get_out_degree() == 1
//         && !current_target->get_accept()) {
//    current_label += current_target->get_edges_cbegin()->first;
//    current_target = current_target->get_edges_cbegin()->second;
//  }
//  if (label != current_label) {
//    add_edge(node, current_target, current_label);
//    remove_edge(node, label);
//  }
//}
//
//std::unordered_map<Node*, size_t> GraphBase::get_target_counts() const
//{
//  std::unordered_map<Node*, size_t> counts;
//  for (Node* node: nodes_) {
//    for (Node* target: node->get_out_targets()) {
//      if (counts.find(target) == counts.end()) {
//        counts[target] = 0;
//      }
//      ++counts[target];
//    }
//  }
//  return counts;
//}

std::shared_ptr<Node> GraphBase::add_unattached_node()
{
  std::shared_ptr<Node> node_ptr(new Node);
  nodes_.insert(node_ptr);
  return node_ptr;
}

bool operator==(const GraphBase& lhs, const GraphBase& rhs)
{
  if (lhs.get_num_nodes() != rhs.get_num_nodes()
      || lhs.get_num_accept() != rhs.get_num_accept()
      || lhs.get_num_edges() != rhs.get_num_edges()) {
    return false;
  }
  std::unordered_map<std::shared_ptr<Node>, std::shared_ptr<Node>> node_table;
  node_table.emplace(lhs.get_root(), rhs.get_root());
  std::queue<std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>>> queue;
  queue.emplace(lhs.get_root(), rhs.get_root());
  while (!queue.empty()) {
    auto current_pair = queue.front();
    if (current_pair.first->get_accept() != current_pair.second->get_accept()) {
      return false;
    }
    queue.pop();
    for (auto lhs_edge_itr = current_pair.first->get_out_edges().begin(),
           rhs_edge_itr = current_pair.second->get_out_edges().begin();
         lhs_edge_itr != current_pair.first->get_out_edges().end()
           && rhs_edge_itr != current_pair.second->get_out_edges().end();
         ++lhs_edge_itr, ++rhs_edge_itr) {
      if (node_table.find(lhs_edge_itr->second) == node_table.end()) {
        node_table.emplace(lhs_edge_itr->second, rhs_edge_itr->second);
        queue.emplace(lhs_edge_itr->second, rhs_edge_itr->second);
      }
      if (lhs_edge_itr->first != rhs_edge_itr->first
          || node_table.at(lhs_edge_itr->second) != rhs_edge_itr->second) {
        return false;
      }
    }
  }
  return true;
}

bool operator!=(const GraphBase& lhs, const GraphBase& rhs)
{
  return !operator==(lhs, rhs);
}
