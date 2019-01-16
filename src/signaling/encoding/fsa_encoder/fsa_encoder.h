/**
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_FSA_ENCODER_H
#define CAPS_FSA_ENCODER_H

// Include standard C++ libraries.
#include <functional>
#include <memory>

// Include other headers from this projects.
#include "../../common/contains.h"
#include "../../common/compare.h"
#include "../../graph/ordering.h"
#include "../../lexicon/fsa_lexicon/fsa_lexicon.h"
#include "../bitvector/bitvector.h"
#include "../coder/string_coder.h"
#include "../coder/binary_coder.h"

/**
 * Base class for a generic FSA encoder.
 *
 * @tparam BitVectorType - the BitVector type to encode and return.
 */
template <typename BitVectorType>
class FSAEncoder
{
 public:

  // Type alias declarations
  using NodeHandle = const Node*;
  using Dist = int;

  // Explicitly disallow default construction.
  FSAEncoder() = delete;

  /**
   * Constructor.
   *
   * Create an FSAEncoder instance from a graph.
   *
   * @param graph - a constant reference to the graph for the encoder to encode.
   */
  explicit FSAEncoder(const FSALexicon& lexicon)
    : lexicon_{lexicon}, node_to_order_{}, order_to_node_{}
  {
    // Nothing to do here.
    auto label_coder =
      std::make_shared<StringCoder<std::string, BitVectorType>>();
    using LabelCoderType = Coder<std::string, BitVectorType>;
    label_coder_ = std::static_pointer_cast<LabelCoderType>(label_coder);

    auto destination_coder =
      std::make_shared<BinaryCoder<int, BitVectorType>>();
    using DestCoderType = Coder<int, BitVectorType>;
    destination_coder_ = std::static_pointer_cast<DestCoderType>(
      destination_coder);
  }

  // Default rule-of-five methods are fine.
  FSAEncoder(const FSAEncoder& orig) = default;
  FSAEncoder(FSAEncoder&& orig) noexcept = default;
  FSAEncoder& operator=(const FSAEncoder& orig) = default;
  FSAEncoder& operator=(FSAEncoder&& orig) noexcept = default;
  virtual ~FSAEncoder() = default;

  /**
   *
   * @return
   */
  virtual BitVectorType encode()
  {
    order_nodes();
    BitVectorType buffer;
    add_header(buffer);
    add_prefix(buffer);
    for (const auto& [index, node]: order_to_node_) {
      auto node_encoding = encode_node(node);
      buffer.push_back(node_encoding);
    }
    add_suffix(buffer);
    return buffer;
  }

  // TODO: add functions to get read-only instances of the lexicon, ordering,
  // and coders

 protected:

  /**
   * Populate the ordering maps of the nodes.
   *
   * This method is idempotent, i.e., it does nothing if the order has
   * already been populated by a previous call to the method.
   */
  virtual void order_nodes()
  {
    // For idempotence, only run if we haven't before
    if (!node_to_order_.empty() || !order_to_node_.empty()) {
      return;
    }

    // Type aliases
    using PairType = std::pair<NodeHandle, Dist>;

    // The DFS explores children by maximum distance from the sink node (desc.)
    auto distance_map = max_distances();

    // Populate DFS stack with root node
    std::stack<NodeHandle> stack;
    stack.emplace(lexicon_.get_graph().get_root());

    // Set up queue for ordering child nodes in the DFS
    std::priority_queue<PairType, std::vector<PairType>,
                        second_greater<NodeHandle, Dist>> queue;

    // Run the DFS.
    while (!stack.empty()) {
      auto node = stack.top();
      auto order_number = node_to_order_.size();
      node_to_order_.emplace(node, static_cast<int>(order_number));
      order_to_node_.emplace(order_number, node);
      stack.pop();
      for (const auto& [label, parent]: node->get_out_edges()) {
        queue.emplace(parent, distance_map.at(parent));
      }
      while (!queue.empty()) {
        stack.emplace(queue.top().first);
        queue.pop();
      }
    }
  }

  virtual void add_header(BitVectorType& buffer)
  {
    (void)buffer;
    // TODO
  }

  virtual void add_prefix(BitVectorType&)
  {
    // Nothing to do here.
  }

  virtual void add_suffix(BitVectorType&)
  {
    // Nothing to do here.
  }

  /**
   * Encode a node in the graph as a bitvector.
   * @param node
   * @return
   */
  virtual BitVectorType encode_node(const Node* node)
  {
    BitVectorType buffer;
    buffer.push_back(node->get_accept());
    for (const auto& [label, parent]: node->get_out_edges()) {
      auto&& edge_encoding = encode_edge(node, parent, label);
      buffer.push_back(*edge_encoding);
    }
    return buffer;
  }

  /**
   * Encode an edge in the graph as a bitvector.
   * @param label
   * @param dst
   * @return
   */
  virtual std::unique_ptr<BitVectorType> encode_edge(
    const Node* src, const Node* dst, const std::string& label)
  {
    // Create an encoding of the label in the buffer.
    auto buffer = std::unique_ptr<BitVectorType>(label_coder_->encode(label));

    // Check if the destination is next from the source in the ordering.
    auto order_diff = FSAEncoder<BitVectorType>::node_to_order_.at(dst) -
                      FSAEncoder<BitVectorType>::node_to_order_.at(src);
    bool next = (order_diff == 1);
    buffer->push_back(next);

    // If the destination does not follow the source, encode the difference.
    if (!next) {
      bool negative = order_diff < 0;
      buffer->push_back(negative);
      auto destination_encoding = std::unique_ptr<BitVectorType>(
        destination_coder_->encode(negative ? -order_diff : order_diff));
      buffer->push_back(*destination_encoding);
    }
    return std::move(buffer);
  }

  /**
   * Find the maximum distance from each node in the internal graph to the
   * destination node.
   * @param dest
   * @return
   */
  std::unordered_map<NodeHandle, int> max_distances() const
  {
    std::unordered_map<NodeHandle, int> map;
    for (const auto& node: reverse_topological_order(lexicon_.get_graph())) {
      if (map.find(node) == map.end()) {
        map.emplace(node, 0);
      }
      for (const auto& [label, parent]: node->get_in_edges()) {
        auto dist = map.at(node) + 1;
        if (map.find(parent) == map.end() || dist > map.at(parent)) {
          map.emplace(parent, dist);
        }
      }
    }
    return map;
  }

  const FSALexicon& lexicon_;

  // TODO: This could be replaced with boost bimap
  std::unordered_map<NodeHandle, int> node_to_order_;
  std::map<int, NodeHandle> order_to_node_;

  std::shared_ptr<Coder<std::string, BitVectorType>> label_coder_;
  std::shared_ptr<Coder<int, BitVectorType>> destination_coder_;
};

#endif //CAPS_FSA_ENCODER_H
