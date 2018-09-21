//
// Created by smaptas on 09/10/18.
//

#ifndef CAPS_FSA_ENCODER_H
#define CAPS_FSA_ENCODER_H

#include <memory>

#include "../common/contains.h"
#include "../lexicon/fsa_lexicon.h"

class FSAEncoder
{
 public:

  explicit FSAEncoder(const FSALexicon& lexicon);

  virtual ~FSAEncoder();

  const FSALexicon& get_lexicon() const;

  virtual std::vector<bool> encode();

 protected:

  /**
   * Populate the ordering maps of the nodes.
   *
   * This method is idempotent, i.e., it does nothing if the order has
   * already been populated by a previous call to the method.
   */
  virtual void order_nodes();

  /**
   * Encode a node in the graph as a bitvector.
   * @param node
   * @return
   */
  virtual std::vector<bool> encode_node(const std::shared_ptr<Node>& node);

  /**
   * Encode an edge in the graph as a bitvector.
   * @param label
   * @param dst
   * @return
   */
  virtual std::vector<bool> encode_edge(const std::shared_ptr<Node>& src,
                                        const std::shared_ptr<Node>& dst,
                                        const std::string& label);

  /**
   * Find the maximum distance from each node in the internal graph to the
   * destination node.
   * @param dest
   * @return
   */
  std::unordered_map<std::shared_ptr<Node>, int> max_distances() const;

  const FSALexicon& lexicon_;

  // TODO: This could be replaced with boost bimap
  std::unordered_map<std::shared_ptr<Node>, int> node_to_order_;
  std::map<int, std::shared_ptr<Node>> order_to_node_;

};

#endif //CAPS_FSA_ENCODER_H
