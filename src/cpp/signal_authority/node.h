#ifndef CAPS_NODE_H_
#define CAPS_NODE_H_

#include <map>
#include <memory>
#include <string>
#include <boost/functional/hash.hpp>

class Node
{
  friend struct NodeHash;
 public:
  // Default constructor.
  Node();

  // The default copy constructor, move constructor, assignment operator, and
  // destructor are fine. Proper handling of Node copying will be handled by the
  // graph.

  bool operator==(const Node& rhs) const;

  bool get_accept() const;
  size_t get_in_degree() const;
  size_t get_out_degree() const;
  std::vector<std::string> get_out_labels() const;
  std::vector<Node*> get_out_targets() const;

  std::map<std::string, Node*>::const_iterator
    get_edges_cbegin() const;
  std::map<std::string, Node*>::const_iterator
    get_edges_cend() const;
  std::map<std::string, Node*>::const_reverse_iterator
    get_edges_crbegin() const;
  std::map<std::string, Node*>::const_reverse_iterator
    get_edges_crend() const;

  const std::map<std::string, Node*>& get_out_edges() const;

  std::string get_last_key() const;
  Node* follow_edge(std::string label) const;

  void set_accept();
  void copy_properties(const Node* rhs);

  void add_in_edge();
  void remove_in_edge();
  void add_out_edge(std::string label, Node* target);
  void remove_out_edge(std::string label);

  std::string to_string() const;

 private:
  bool accept_;
  size_t in_degree_;
  std::map<std::string, Node*> edges_;
};

/**
 * Functor to calculate the hash of a node.
 */
struct NodeHash
{
  std::size_t operator()(const Node* node) const;
};

/**
 * Functor to determine if two node pointers are equivalent.
 */
struct NodePred
{
  bool operator()(const Node* node1,
                  const Node* node2) const;
};

#endif  // CAPS_NODE_H_
