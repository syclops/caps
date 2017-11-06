#ifndef CAPS_NODE_H_
#define CAPS_NODE_H_

#include <map>
#include <memory>
#include <string>

#include <boost/functional/hash.hpp>

// Forward declare the Node class for the custom comparator and equality
// predicate.
class Node;

struct in_edge_comparator
{
  bool operator()(const std::pair<std::string, std::weak_ptr<Node>>& lhs,
                  const std::pair<std::string, std::weak_ptr<Node>>& rhs) const;
};

bool in_edge_equal(const std::pair<std::string, std::weak_ptr<Node>>& lhs,
                   const std::pair<std::string, std::weak_ptr<Node>>& rhs);

class Node
{
  friend struct NodeHash;

 private:
  bool accept_;
  std::set<std::pair<std::string, std::weak_ptr<Node>>,
           in_edge_comparator> in_edges_;
  std::map<std::string, std::shared_ptr<Node>> out_edges_;

 public:

  // Default constructor.
  Node();

  // The default copy constructor, move constructor, assignment operator, and
  // destructor are fine. Proper memory management will be handled by the graph.

  bool get_accept() const noexcept;
  int get_in_degree() const noexcept;
  int get_out_degree() const noexcept;
  std::string get_last_out_label() const;

  bool has_in_edge(std::string label, std::weak_ptr<Node> source) const;
  bool has_out_edge(std::string label, std::shared_ptr<Node> source) const;
  bool has_out_label(std::string label) const;

  void set_accept(bool accept) noexcept;

  void add_in_edge(std::string label, std::weak_ptr<Node> source);
  void remove_in_edge(std::string label, std::weak_ptr<Node> source);
  void add_out_edge(std::string label, std::shared_ptr<Node> target);
  void remove_out_edge(std::string label);

  std::shared_ptr<Node> follow_out_edge(std::string label) const;

  template <typename IterableType> class EdgeIterable
  {
   public:

    EdgeIterable(const IterableType& map)
      : map_(map)
    {
      // Nothing to do here.
    }

    typename IterableType::const_iterator begin() const
    {
      return map_.cbegin();
    }

    typename IterableType::const_iterator end() const
    {
      return map_.cend();
    }

   private:
    const IterableType& map_;
  };

  template <typename ReverseIterableType> class EdgeReverseIterable
  {
   public:

    EdgeReverseIterable(const ReverseIterableType& map)
      : map_(map)
    {
      // Nothing to do here.
    }

    typename ReverseIterableType::const_reverse_iterator begin() const
    {
      return map_.crbegin();
    }

    typename ReverseIterableType::const_reverse_iterator end() const
    {
      return map_.crend();
    }

   private:
    const ReverseIterableType& map_;
  };

  friend EdgeIterable<decltype(in_edges_)>;
  friend EdgeIterable<decltype(out_edges_)>;
  friend EdgeReverseIterable<decltype(in_edges_)>;
  friend EdgeReverseIterable<decltype(out_edges_)>;

  EdgeIterable<decltype(in_edges_)> get_in_edges() const;
  EdgeIterable<decltype(out_edges_)> get_out_edges() const;
  EdgeReverseIterable<decltype(in_edges_)> get_reverse_in_edges() const;
  EdgeReverseIterable<decltype(out_edges_)> get_reverse_out_edges() const;
};

bool operator==(const Node& lhs, const Node& rhs);

bool operator!=(const Node& lhs, const Node& rhs);

/**
 * Functor to calculate the hash of a node.
 */
struct NodeHash
{
  std::size_t operator()(const std::shared_ptr<Node>& node) const;
};

/**
 * Functor to determine if two node pointers are equivalent.
 */
struct NodePred
{
  bool operator()(const std::shared_ptr<Node>& lhs,
                  const std::shared_ptr<Node>& rhs) const;
};

#endif  // CAPS_NODE_H_
