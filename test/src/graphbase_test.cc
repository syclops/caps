/**
 * Author: smaptas
 */


// Include header file.

// Include C standard libraries.

// Include C++ standard libraries.

// Include other headers from this project.
#include "../../src/cpp/graph/graph.h"
#include "../../src/cpp/graph/node.h"

// Include header from other projects.
#include "gtest/gtest.h"

TEST(GraphBase, EmptyGraph)
{
  const LabeledGraph graph;
  EXPECT_EQ(1, graph.get_num_nodes());
  EXPECT_EQ(0, graph.get_num_edges());
  EXPECT_EQ(0, graph.get_num_accept());
  EXPECT_FALSE(graph.get_compacted());
}

TEST(GraphBase, GraphReflexiveEqual)
{
  const LabeledGraph graph;
  EXPECT_EQ(graph.get_root(), graph.get_root());
  EXPECT_EQ(graph, graph);
}

TEST(GraphBase, EmptyGraphsEqual)
{
  const LabeledGraph graph1, graph2;
  EXPECT_NE(graph1.get_root(), graph2.get_root());
  EXPECT_EQ(graph1, graph2);
}

TEST(GraphBase, CopyEmptyGraph)
{
  const LabeledGraph graph1;
  const LabeledGraph graph2(graph1);
  EXPECT_NE(graph1.get_root(), graph2.get_root());
  EXPECT_EQ(graph1, graph2);
}

const auto SAMPLE_LABEL = "a";

TEST(GraphBase, GraphsNotEqual)
{
  LabeledGraph graph1, graph2;
  graph1.add_edge(graph1.get_root(), SAMPLE_LABEL);
  EXPECT_NE(graph1, graph2);
}

//TODO: test more pathological unequal graphs

// We don't test the add_node function since it's an alias for add_edge with the
// same parameters.

TEST(GraphBase, RemoveNode)
{
  const LabeledGraph empty_graph;
  LabeledGraph graph;
  auto node = graph.add_node(graph.get_root(), SAMPLE_LABEL);
  graph.remove_node(node);
  EXPECT_EQ(empty_graph, graph);
}

// TODO: test removing a node not in the graph

TEST(GraphBase, RemoveNodeRecursive)
{
  LabeledGraph graph;
  auto child = graph.add_node(graph.get_root(), SAMPLE_LABEL);
  auto grandchild = graph.add_node(child, SAMPLE_LABEL);
  EXPECT_EQ(2, graph.get_num_edges());
  graph.remove_node(child);
  EXPECT_EQ(1, graph.get_num_nodes());
  EXPECT_EQ(0, graph.get_num_edges());
}

const auto SAMPLE_LABEL2 = "b";
const auto SAMPLE_LABEL3 = "c";

TEST(GraphBase, RemoveNodeParallel)
{
  LabeledGraph graph;
  auto child = graph.add_node(graph.get_root(), SAMPLE_LABEL);
  auto grandchild1 = graph.add_node(child, SAMPLE_LABEL);
  graph.add_edge(child, grandchild1, SAMPLE_LABEL2);
  auto grandchild2 = graph.add_node(child, SAMPLE_LABEL3);
  graph.add_edge(graph.get_root(), grandchild2, SAMPLE_LABEL2);
  graph.remove_node(child);
  EXPECT_EQ(2, graph.get_num_nodes());
  EXPECT_EQ(1, graph.get_num_edges());
}

TEST(GraphBase, RemoveNodePreservesEqual)
{
  LabeledGraph graph1, graph2;
  auto node1 = graph1.add_node(graph1.get_root(), SAMPLE_LABEL);
  auto node2 = graph2.add_node(graph2.get_root(), SAMPLE_LABEL);
  graph1.remove_node(node1);
  graph2.remove_node(node2);
  EXPECT_EQ(graph1, graph2);
}

TEST(GraphBase, AddEdgeWithoutDest)
{
  LabeledGraph graph;
  graph.add_edge(graph.get_root(), SAMPLE_LABEL);
  EXPECT_EQ(2, graph.get_num_nodes());
  EXPECT_EQ(1, graph.get_num_edges());
}

// TODO: test adding an edge with a source node not in the graph

TEST(GraphBase, AddEdgeWithoutDestPreservesEqual)
{
  LabeledGraph graph1, graph2;
  graph1.add_edge(graph1.get_root(), SAMPLE_LABEL);
  graph2.add_edge(graph2.get_root(), SAMPLE_LABEL);
  EXPECT_EQ(graph1, graph2);
}

TEST(GraphBase, RemoveEdge)
{
  LabeledGraph graph;
  graph.add_edge(graph.get_root(), SAMPLE_LABEL);
  graph.remove_edge(graph.get_root(), SAMPLE_LABEL);
  EXPECT_EQ(0, graph.get_num_edges());
}

// TODO: test removing edge with source node not in the graph
// TODO: test removing edge with label not in out-edge labels
// TODO: test removing edge to child with in-degree 1

// TODO: test iterating over nodes
