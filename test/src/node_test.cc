/**
 * Unit test the Node class.
 *
 * Author: Steve Matsumoto
 *
 * Notes:
 *
 * The following methods are still untested:
 *   * get_last_out_label()
 *   * follow_out_edge()
 */

// Include other headers from this project.
#include "../../src/cpp/common/contains.h"
#include "../../src/cpp/common/iterable.h"
#include "../../src/cpp/graph/node.h"

// Include headers; from other projects.
#include "gtest/gtest.h"


// Set up useful test fixtures that can be derived from for each test suite.

/**
 * Provide an empty node that can be used as a reference.
 */
class EmptyNode: public virtual testing::Test
{
 protected:
  const Node empty_node_;
};

/**
 * Provide a sample non-const test node.
 */
class TestNode: public virtual testing::Test
{
 protected:
  Node test_node_;
};

/**
 * Provide a second non-const test node if two are needed.
 */
class TwoTestNodes: public virtual TestNode
{
 protected:
  Node other_test_node_;
};

/**
 * Provide a label to use in tests.
 */
class TestLabel: public virtual testing::Test
{
 protected:
  const std::string label_ = "a";
};

/**
 * Provide an additional label for tests involving creation of edges with
 * different labels.
 */
class TwoTestLabels: public virtual TestLabel
{
 protected:
  const std::string label2_ = "b";
};

/**
 * Provide a node to serve as the target for an out-edge.
 */
class AddOutEdge: public virtual TestNode, public virtual TestLabel
{
 protected:

  virtual void SetUp()
  {
    add_node_shared_ptr_ = std::shared_ptr<Node>{new Node};
  }

  std::shared_ptr<Node> add_node_shared_ptr_;
};

/**
 * Provide a weak pointer to a node to serve as the source for an in-edge.
 *
 * The weak pointer is a copy of the shared pointer provided by AddOutEdge.
 */
class AddInEdge: public virtual AddOutEdge
{
 protected:

  virtual void SetUp()
  {
    AddOutEdge::SetUp();
    add_node_weak_ptr_ = {add_node_shared_ptr_};
  }

  std::weak_ptr<Node> add_node_weak_ptr_;
};


// Begin tests here. We may define additional fixtures that inherit from those
// above.

/**
 * Test fixture for constructing nodes.
 */
class NodeTestCtor: public EmptyNode
{
  virtual void SetUp()
  {
    EmptyNode::SetUp();
  }
};

/**
 * Check that an empty (default-constructed) node has the appropriate default
 * values.
 */
TEST_F(NodeTestCtor, DefaultCtorValues)
{
  EXPECT_FALSE(empty_node_.get_accept());
  EXPECT_EQ(0, empty_node_.get_in_degree());
  EXPECT_EQ(0, empty_node_.get_out_degree());
}

/**
 * Test fixture for testing a node's accept status.
 */
class NodeTestAccept: public TestNode
{
  virtual void SetUp()
  {
    TestNode::SetUp();
  }
};

/**
 * Check that setting a node's accept state works as expected.
 */
TEST_F(NodeTestAccept, SetAccept)
{
  test_node_.set_accept(true);
  EXPECT_TRUE(test_node_.get_accept());
  test_node_.set_accept(false);
  EXPECT_FALSE(test_node_.get_accept());
}

/**
 * Test fixture for operations relating to in-edges.
 */
class NodeTestInEdge: public AddInEdge, public TwoTestLabels
{
  /**
   * Add an in-edge to the test node.
   */
  virtual void SetUp()
  {
    AddInEdge::SetUp();
    TwoTestLabels::SetUp();
    test_node_.add_in_edge(label_, add_node_weak_ptr_);
  }
};

/**
 * Check that adding an in-edge increments the node's in-degree and inserts the
 * correct information into the in-edge set.
 */
TEST_F(NodeTestInEdge, AddInEdge)
{
  EXPECT_EQ(1, test_node_.get_in_degree());
  EXPECT_TRUE(test_node_.has_in_edge(label_, add_node_weak_ptr_));
}

/**
 * Check that adding an in-edge is idempotent (i.e., adding the same edge twice
 * leaves the node unchanged the second time).
 */
TEST_F(NodeTestInEdge, AddInEdgeIdempotent)
{
  test_node_.add_in_edge(label_, add_node_weak_ptr_);
  EXPECT_EQ(1, test_node_.get_in_degree());
}

/**
 * Check that removing an in-edge decrements the node's in-degree and deletes
 * the correct information into the in-edge set.
 */
TEST_F(NodeTestInEdge, RemoveInEdge)
{
  test_node_.remove_in_edge(label_, add_node_weak_ptr_);
  EXPECT_EQ(0, test_node_.get_in_degree());
  EXPECT_FALSE(test_node_.has_in_edge(label_, add_node_weak_ptr_));
  // TODO: check that no other in-edges have changed.
}

/**
 * Check that removing an in-edge is idempotent (i.e., removing the same edge
 * twice leaves the node unchanged the second time).
 */
TEST_F(NodeTestInEdge, RemoveInEdgeIdempotent)
{
  test_node_.remove_in_edge(label_, add_node_weak_ptr_);
  test_node_.remove_in_edge(label_, add_node_weak_ptr_);
  EXPECT_EQ(0, test_node_.get_in_degree());
}

/**
 * Check that attempting to remove an in-edge not present in the node's in-edge
 * set leaves the node unchanged.
 */
TEST_F(NodeTestInEdge, RemoveNonexistentInEdge)
{
  test_node_.remove_in_edge(label2_, add_node_weak_ptr_);
  EXPECT_EQ(1, test_node_.get_in_degree());
  EXPECT_TRUE(test_node_.has_in_edge(label_, add_node_weak_ptr_));
}

/**
 * Check that iterating through the in-edges of a node proceeds in the expected
 * order.
 */
TEST_F(NodeTestInEdge, IterateInEdges)
{
  test_node_.add_in_edge(label2_, add_node_weak_ptr_);
  const std::vector<std::pair<std::string, std::weak_ptr<Node>>> edges{
    {label_, add_node_weak_ptr_}, {label2_, add_node_weak_ptr_}};
  auto i = 0;
  for (const auto in_edge_itr: test_node_.get_in_edges()) {
    EXPECT_EQ(in_edge_itr.first, edges[i].first);
    EXPECT_EQ(in_edge_itr.second.lock(), edges[i].second.lock());
    ++i;
  }
}

/**
 * Check that reverse iterating through the in-edges of a node proceeds in the
 * expected order.
 */
TEST_F(NodeTestInEdge, IterateReverseInEdges)
{
  test_node_.add_in_edge(label2_, add_node_weak_ptr_);
  const std::vector<std::pair<std::string, std::weak_ptr<Node>>> edges{
    {label2_, add_node_weak_ptr_}, {label_, add_node_weak_ptr_}};
  auto i = 0;
  for (const auto in_edge_itr: test_node_.get_reverse_in_edges()) {
    EXPECT_EQ(in_edge_itr.first, edges[i].first);
    EXPECT_EQ(in_edge_itr.second.lock(), edges[i].second.lock());
    ++i;
  }
}

// TODO: check iteration for more complex orders.

/**
 * Test fixture for operations relating to out-edges.
 */
class NodeTestOutEdge: public AddOutEdge, public TwoTestLabels
{
  virtual void SetUp()
  {
    AddOutEdge::SetUp();
    TwoTestLabels::SetUp();
    test_node_.add_out_edge(label_, add_node_shared_ptr_);
  }
};

/**
 * Check that adding an out-edge increments the node's out-degree and inserts
 * the correct information into the out-edge set.
 */
TEST_F(NodeTestOutEdge, AddOutEdgeDegree)
{
  EXPECT_EQ(1, test_node_.get_out_degree());
  EXPECT_TRUE(test_node_.has_out_edge(label_, add_node_shared_ptr_));
}

/**
 * Check that adding an out-edge is idempotent (i.e., adding the same edge twice
 * leaves the node unchanged the second time).
 */
TEST_F(NodeTestOutEdge, AddOutEdgeIdempotent)
{
  test_node_.add_out_edge(label_, add_node_shared_ptr_);
  EXPECT_EQ(1, test_node_.get_out_degree());
  EXPECT_TRUE(test_node_.has_out_edge(label_, add_node_shared_ptr_));
}

/**
 * Check that removing an out-edge decrements the node's out-degree and deletes
 * the correct information into the out-edge set.
 */
TEST_F(NodeTestOutEdge, RemoveOutEdge)
{
  test_node_.remove_out_edge(label_);
  EXPECT_EQ(0, test_node_.get_out_degree());
  EXPECT_FALSE(test_node_.has_out_edge(label_, add_node_shared_ptr_));
}

/**
 * Check that removing an out-edge is idempotent (i.e., removing the same edge
 * twice leaves the node unchanged the second time).
 */
TEST_F(NodeTestOutEdge, RemoveOutEdgeIdempotent)
{
  test_node_.remove_out_edge(label_);
  test_node_.remove_out_edge(label_);
  EXPECT_EQ(0, test_node_.get_out_degree());
  EXPECT_FALSE(test_node_.has_out_edge(label_, add_node_shared_ptr_));
}

/**
 * Check that attempting to remove an out-edge not present in the node's
 * out-edge set leaves the node unchanged.
 */
TEST_F(NodeTestOutEdge, RemoveNonexistentOutEdge)
{
  test_node_.remove_out_edge(label2_);
  EXPECT_TRUE(test_node_.has_out_edge(label_, add_node_shared_ptr_));
}

/**
 * Check that iterating through the out-edges of a node proceeds in the expected
 * order.
 */
TEST_F(NodeTestOutEdge, IterateOutEdges)
{
  test_node_.add_out_edge(label2_, add_node_shared_ptr_);
  const std::vector<std::pair<std::string, std::shared_ptr<Node>>> edges{
    {label_, add_node_shared_ptr_}, {label2_, add_node_shared_ptr_}};
  auto i = 0;
  for (auto& out_edge_itr: test_node_.get_out_edges()) {
    EXPECT_EQ(out_edge_itr.first, edges[i].first);
    EXPECT_EQ(out_edge_itr.second, edges[i].second);
    ++i;
  }
}

/**
 * Check that reverse iterating through the out-edges of a node proceeds in the
 * expected order.
 */
TEST_F(NodeTestOutEdge, IterateReverseOutEdges)
{
  test_node_.add_out_edge(label2_, add_node_shared_ptr_);
  const std::vector<std::pair<std::string, std::shared_ptr<Node>>> edges{
    {label2_, add_node_shared_ptr_}, {label_, add_node_shared_ptr_}};
  auto i = 0;
  for (auto& out_edge_itr: test_node_.get_reverse_out_edges()) {
    EXPECT_EQ(out_edge_itr.first, edges[i].first);
    EXPECT_EQ(out_edge_itr.second, edges[i].second);
    ++i;
  }
}

// TODO: check iteration for more complex orders.

//TEST(Node, IterateOutNodes)
//{
//  Node node;
//  std::shared_ptr<Node> child1{new Node}, child2{new Node}, child3{new Node};
//  const std::vector<std::pair<std::string, std::shared_ptr<Node>>> edges{
//    {"1", child1}, {"2", child2}, {"3", child3}
//  };
//  for (const auto& edge: edges) {
//    node.add_out_edge(edge.first, edge.second);
//  }
//  auto i = 0;
//  for (auto& out_edge_itr: node.get_out_edges()) {
//    EXPECT_EQ(out_edge_itr.first, edges[i].first);
//    EXPECT_EQ(out_edge_itr.second, edges[i].second);
//    ++i;
//  }
//}
//
//TEST(Node, IterateReverseOutNodes)
//{
//  Node node;
//  std::shared_ptr<Node> child1{new Node}, child2{new Node}, child3{new Node};
//  const std::vector<std::pair<std::string, std::shared_ptr<Node>>> edges{
//    {"3", child3}, {"2", child2}, {"1", child1}
//  };
//  for (const auto& edge: edges) {
//    node.add_out_edge(edge.first, edge.second);
//  }
//  auto i = 0;
//  for (auto& out_edge_itr: node.get_reverse_out_edges()) {
//    EXPECT_EQ(out_edge_itr.first, edges[i].first);
//    EXPECT_EQ(out_edge_itr.second, edges[i].second);
//    ++i;
//  }
//}
/**
 * Test fixture for testing node equality under different operations.
 */
class NodeTestEqual: public TwoTestNodes, public AddInEdge, public TwoTestLabels
{
 protected:
  virtual void SetUp()
  {
    TwoTestNodes::SetUp();
    AddInEdge::SetUp();
    TwoTestLabels::SetUp();
  }
};

/**
 * Check that a node is equal to itself.
 */
TEST_F(NodeTestEqual, Reflexive)
{
  EXPECT_EQ(test_node_, test_node_);
}

/**
 * Check that two different empty nodes are equal.
 */
TEST_F(NodeTestEqual, EmptyNodesEqual)
{
  EXPECT_EQ(test_node_, other_test_node_);
}

/**
 * Check that equality depends on the accept status of a node.
 */
TEST_F(NodeTestEqual, AcceptEqual)
{
  // Test that the accept state affects equality.
  test_node_.set_accept(true);
  EXPECT_NE(test_node_, other_test_node_);
  other_test_node_.set_accept(true);
  EXPECT_EQ(test_node_, other_test_node_);
}

/**
 * Check that equality depends on the in-edges of a node.
 */
TEST_F(NodeTestEqual, AddInEdge)
{
  test_node_.add_in_edge(label_, add_node_weak_ptr_);
  EXPECT_NE(test_node_, other_test_node_);
  other_test_node_.add_in_edge(label_, add_node_weak_ptr_);
  EXPECT_EQ(test_node_, other_test_node_);
}

/**
 * Check that equality does NOT hold when the nodes have the same number of
 * in-edges, but these in-edges are different.
 */
TEST_F(NodeTestEqual, AddDifferentInEdges)
{
  test_node_.add_in_edge(label_, add_node_weak_ptr_);
  other_test_node_.add_in_edge(label2_, add_node_weak_ptr_);
  EXPECT_NE(test_node_, other_test_node_);
}

/**
 * Check that equality depends on the out-edges of a node.
 */
TEST_F(NodeTestEqual, AddOutEdge)
{
  test_node_.add_out_edge(label_, add_node_shared_ptr_);
  EXPECT_NE(test_node_, other_test_node_);
  other_test_node_.add_out_edge(label_, add_node_shared_ptr_);
  EXPECT_EQ(test_node_, other_test_node_);
}

/**
 * Check that equality does NOT hold when the nodes have the same number of
 * out-edges, but these out-edges are different.
 */
TEST_F(NodeTestEqual, AddDifferentOutEdges)
{
  test_node_.add_out_edge(label_, add_node_shared_ptr_);
  other_test_node_.add_out_edge(label2_, add_node_shared_ptr_);
  EXPECT_NE(test_node_, other_test_node_);
}

