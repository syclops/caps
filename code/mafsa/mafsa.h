#ifndef POLISIG_MAFSA_H_
#define POLISIG_MAFSA_H_

#include <iostream>
#include <map>
#include <vector>

class Graph
{
  //struct Node;

 public:

  Graph();
  Graph(const Graph& orig);
  Graph& operator=(const Graph& rhs);

  ~Graph();

  void read_file(std::istream& input_stream);

  size_t num_strings() const;
  size_t num_states() const;
  size_t num_bytes() const;
  size_t num_edges() const;

  std::string to_string() const;
  std::string to_dot() const;

 private:

  struct Node
  {
    Node();

    bool operator==(const Node& rhs) const;

    bool has_children() const;
    Node* last_edge() const;
    std::string to_string() const;

    bool accept;
    std::map<char, Node*> edges;
  };

  void add_string(const std::string str);

  void replace_or_register(Node* node);

  size_t num_strings_;
  size_t num_states_;
  size_t num_bytes_;
  size_t num_edges_;
  std::vector<Node*> register_;
  Node* root_;
};

std::ostream& operator<<(std::ostream& out, const Graph& graph);

#endif  // POLISIG_MAFSA_H_
