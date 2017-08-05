#ifndef POLISIG_MAFSA_H_
#define POLISIG_MAFSA_H_

#include <iostream>
#include <map>
#include <memory> // shared_ptr
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <boost/functional/hash.hpp>

class SignalSet
{
 public:

  // Default constructor.
  SignalSet();

  // Copy constructor.
  SignalSet(const SignalSet& orig);

  // Move constructor.
  SignalSet(SignalSet&& orig);

  // Assignment operator (for both copy-assign and move-assign).
  SignalSet& operator=(SignalSet rhs);

  // Destructor.
  ~SignalSet();

  // Swap function.
  friend void swap(SignalSet& first, SignalSet& second);

  /**
   * Read in a list of strings from a file.
   *
   * Given an istream representing an input file, populate the signal set with
   */
  void read_file(std::istream& input_stream);

  void dump_dictionary(std::ofstream& outfile) const;

  void compact_edges();

  bool check_word(std::string word) const;

  size_t num_strings() const;
  size_t num_bytes() const;
  size_t num_states() const;
  size_t num_edges() const;

  /**
   * Return a human-readable string representation of the graph.
   */
  std::string to_string() const;

  /**
   * Return a string containing the DOT representation of the graph.
   */
  std::string to_dot() const;

  /**
   * Print a DOT representation of the graph to a file stream.
   */
  void print_dot(std::ofstream& outfile) const;

 private:

  struct Node
  {
    // Default constructor.
    Node();

    // The default copy constructor, assignment operator, and destructor are
    // fine.

    bool operator==(const Node& rhs) const;

    bool has_children() const;
    Node* last_edge() const;
    std::string to_string() const;
    inline void join(std::string label, Node* target);
    void compact_edge(std::string label);

    bool accept;
    size_t in_degree;
    std::map<std::string, Node*> edges;
  };

  struct NodeHash
  {
    std::size_t operator()(const Node* node) const
    {
      std::size_t seed = 0;
      boost::hash_combine(seed, node->accept);
      for (auto it = node->edges.begin(); it != node->edges.end(); ++it) {
        boost::hash_combine(seed, it->first);
        boost::hash_combine(seed, it->second);
      }
      return seed;
    }
  };

  struct NodePred
  {
    bool operator()(const Node* node1, const Node* node2) const
    {
      return *node1 == *node2;
    }
  };

  void add_string(const std::string str);

  /**
   *
   */
  void replace_or_register(Node* node);

  std::pair<std::string, Node*> match(Node* node, std::string word) const;

  size_t num_strings_;
  size_t num_bytes_;
  size_t num_states_;
  size_t num_edges_;

  bool compacted_;

  /**
   * Register for finding equivalent states during FSA construction.
   */
  std::unordered_set<Node*, NodeHash, NodePred> register_;

  /**
   * Pointer to the root node.
   */
  Node* root_;
};

std::ostream& operator<<(std::ostream& out, const SignalSet& graph);

#endif  // POLISIG_MAFSA_H_
