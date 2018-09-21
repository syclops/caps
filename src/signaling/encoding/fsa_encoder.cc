//
// Created by smaptas on 09/10/18.
//

#include "fsa_encoder.h"
#include "bitstream.h"
#include "../common/compare.h"
#include "../graph/ordering.h"
#include "../lexicon/fsa_lexicon.h"

#include <boost/heap/fibonacci_heap.hpp>
#include <queue>

FSAEncoder::FSAEncoder(const FSALexicon& lexicon)
  : lexicon_{lexicon}, node_to_order_{}, order_to_node_{}
{
  // Nothing to do here.
}

FSAEncoder::~FSAEncoder()
{
  // Nothing to do here.
}

const FSALexicon& FSAEncoder::get_lexicon() const
{
  return lexicon_;
}

std::vector<bool> FSAEncoder::encode()
{
  order_nodes();
  std::vector<bool> buffer;
  for (const auto& order_node: order_to_node_) {
    auto node_encoding = encode_node(order_node.second);
    buffer.reserve(buffer.size() + node_encoding.size());
    for (auto bit: node_encoding) {
      buffer.push_back(bit);
    }
  }
  return buffer;
}

void FSAEncoder::order_nodes()
{
  // For idempotence, only run if we haven't before
  if (!node_to_order_.empty() || !order_to_node_.empty()) {
    return;
  }

  // Type aliases
  using PtrType = std::shared_ptr<Node>;
  using DistType = int;
  using PairType = std::pair<PtrType, DistType>;

  // The DFS explores children by maximum distance from the sink node (desc.)
  auto distance_map = max_distances();

  // Populate DFS stack with root node
  std::stack<PtrType> stack;
  stack.push(lexicon_.get_graph().get_root());

  // Set up queue for ordering child nodes in the DFS
  std::priority_queue<PairType, std::vector<PairType>,
                      first_greater<PtrType, DistType>> queue;

  while (!stack.empty()) {
    auto node = stack.top();
    auto order_number = node_to_order_.size();
    node_to_order_[node] = static_cast<int>(order_number);
    order_to_node_[order_number] = node;
    stack.pop();
    for (const auto& out_edge: node->get_out_edges()) {
      queue.push(std::make_pair(out_edge.second,
                                distance_map.at(out_edge.second)));
    }
    while (!queue.empty()) {
      stack.push(queue.top().first);
      queue.pop();
    }
  }
}

std::vector<bool> FSAEncoder::encode_node(const std::shared_ptr<Node>& node)
{
  std::vector<bool> buffer;
  buffer.push_back(node->get_accept());
  for (auto out_edge: node->get_out_edges()) {
    auto edge_encoding = encode_edge(node, out_edge.second, out_edge.first);
    for (auto bit: edge_encoding) {
      buffer.push_back(bit);
    }
  }
  return buffer;
}

std::vector<bool> FSAEncoder::encode_edge(const std::shared_ptr<Node>& src,
                                          const std::shared_ptr<Node>& dst,
                                          const std::string& label)
{
  BitStream bstream;
  bstream << label;
  bool next = (node_to_order_.at(dst) - node_to_order_.at(src) == 1);
  bstream << next;
  if (!next) {
    bstream.append(node_to_order_.at(dst),
                   static_cast<int>(ceil(log2(node_to_order_.size()))));
  }
  return bstream.dump();
}

std::unordered_map<std::shared_ptr<Node>, int> FSAEncoder::max_distances() const
{
  std::unordered_map<std::shared_ptr<Node>, int> map;
  for (const auto& node: reverse_topological_order(lexicon_.get_graph())) {
    if (map.empty()) {
      map[node] = 0;
    }
    for (const auto& in_edge: node->get_in_edges()) {
      auto dist = map[node] + 1;
      if (!contains(map, node) || dist > map.at(node)) {
        map[in_edge.second.lock()] = dist;
      }
    }
  }
  return map;
}

