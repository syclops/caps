#ifndef POLISIG_DAFSA_H_
#define POLISIG_DAFSA_H_

#include <iostream>
#include <string>
#include <unordered_set>
#include "../graph/graph.h"
#include "../graph/node.h"

class DAFSA: public LabeledGraph
{
 public:

  /**
   * Default constructor.
   */
  DAFSA() = default;

  /**
   * Copy constructor.
   * @param orig
   */
  DAFSA(const DAFSA& orig);

  /**
   * Move constructor.
   * @param orig
   */
  DAFSA(DAFSA&& orig) noexcept;

  /**
   * Copy and move assignment operator.
   * @param rhs
   * @return
   */
  DAFSA& operator=(DAFSA rhs);

  /**
   * Destructor.
   */
  virtual ~DAFSA() = default;

  /**
   * Swap two instances of a DAFSA.
   * @param first
   * @param second
   */
  friend void swap(DAFSA& first, DAFSA& second);

  /**
   * Read a file stream of strings to add to the DAFSA.
   * @param in_stream
   */
  void read_file(std::istream& in_stream) override;

  /**
   * Dump the binary representation of the DAFSA.
   * @param out_stream
   */
  void dump_bin(std::ostream& out_stream) const override;

  /**
   * Load a binary representation of a DAFSA from a file stream.
   * @param in_stream
   */
  void load_bin(std::istream& in_stream) override;

 protected:

  /**
   * Replace a node with an equivalent node from the register or add the node to
   * the register.
   * @param node
   */
  void replace_or_register(Node* node);

  /**
   * Add a single string to the DAFSA.
   * @param str
   */
  void add_string(std::string str) override;

  std::unordered_set<Node*, NodeHash, NodePred> register_;
};

#endif  // POLISIG_DAFSA_H_
