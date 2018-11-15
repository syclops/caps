/**
 * Author: smaptas
 */


// Include header file.

// Include C standard libraries.

// Include C++ standard libraries.

// Include other headers from this project.
#include "../../../src/signaling/graph/labeled_graph/graph.h"
#include "../../../src/signaling/graph/node/node.h"

// Include header from other projects.
#include "gtest/gtest.h"

TEST(LabeledGraph, EmptyGraph)
{
  const LabeledGraph graph;
  EXPECT_EQ(1, graph.get_num_nodes());
  EXPECT_EQ(0, graph.get_num_edges());
  EXPECT_EQ(0, graph.get_num_accept());
  EXPECT_FALSE(graph.get_compacted());
  EXPECT_EQ(1, graph.get_nodes().size());
  EXPECT_TRUE(graph.get_source_counts().empty());
  EXPECT_TRUE(graph.get_dest_counts().empty());
  EXPECT_TRUE(graph.get_label_counts().empty());
}

TEST(LabeledGraph, GraphReflexiveEqual)
{
  const LabeledGraph graph;
  EXPECT_EQ(graph, graph);
}

TEST(LabeledGraph, EmptyGraphsEqual)
{
  const LabeledGraph graph1, graph2;
  EXPECT_EQ(graph1, graph2);
}

TEST(LabeledGraph, CopyEmptyGraph)
{
  const LabeledGraph graph1;
  const LabeledGraph graph2{graph1};
  EXPECT_EQ(graph1, graph2);
}

const auto SAMPLE_LABEL = "a";

TEST(LabeledGraph, CopyNonEmptyGraph)
{
  LabeledGraph graph1;
  graph1.add_edge(graph1.get_root(), SAMPLE_LABEL);
  LabeledGraph graph2{graph1};
  EXPECT_EQ(graph1.get_num_nodes(), graph2.get_num_nodes());
  EXPECT_EQ(graph1.get_num_edges(), graph2.get_num_edges());
  EXPECT_EQ(graph1.get_num_accept(), graph2.get_num_accept());
  EXPECT_EQ(graph1.get_compacted(), graph2.get_compacted());
  EXPECT_EQ(graph1.get_source_counts().size(),
            graph2.get_source_counts().size());
  EXPECT_EQ(graph1.get_dest_counts().size(),
            graph2.get_dest_counts().size());
  EXPECT_EQ(graph1.get_label_counts().size(),
            graph2.get_label_counts().size());
  EXPECT_EQ(graph1, graph2);
}

TEST(LabeledGraph, GraphsNotEqual)
{
  LabeledGraph graph1, graph2;
  graph1.add_edge(graph1.get_root(), SAMPLE_LABEL);
  EXPECT_NE(graph1, graph2);
}

//TODO: tests more pathological unequal graphs

TEST(LabeledGraph, SetAccept)
{
  LabeledGraph graph;
  graph.set_accept(graph.get_root(), true);
  EXPECT_EQ(1, graph.get_num_nodes());
  EXPECT_EQ(0, graph.get_num_edges());
  EXPECT_EQ(1, graph.get_num_accept());
  EXPECT_FALSE(graph.get_compacted());
  EXPECT_EQ(1, graph.get_nodes().size());
  EXPECT_TRUE(graph.get_source_counts().empty());
  EXPECT_TRUE(graph.get_dest_counts().empty());
  EXPECT_TRUE(graph.get_label_counts().empty());
}

// We don't tests the add_node function since it's an alias for add_edge with the
// same parameters.

TEST(LabeledGraph, RemoveNode)
{
  const LabeledGraph empty_graph;
  LabeledGraph graph;
  auto node = graph.add_node(graph.get_root(), SAMPLE_LABEL);
  graph.remove_node(node);
  EXPECT_EQ(1, graph.get_num_nodes());
  EXPECT_EQ(0, graph.get_num_edges());
  EXPECT_EQ(0, graph.get_num_accept());
  EXPECT_FALSE(graph.get_compacted());
  EXPECT_EQ(1, graph.get_nodes().size());
  EXPECT_TRUE(graph.get_source_counts().empty());
  EXPECT_TRUE(graph.get_dest_counts().empty());
  EXPECT_TRUE(graph.get_label_counts().empty());
  EXPECT_EQ(empty_graph, graph);
}

TEST(LabeledGraph, RemoveNonMemberNode)
{
  const LabeledGraph empty_graph;
  LabeledGraph graph;
  auto non_member = std::make_unique<Node>();
  graph.remove_node(non_member.get());
  EXPECT_EQ(empty_graph, graph);
}

TEST(LabeledGraph, RemoveAcceptNode)
{
  LabeledGraph graph;
  auto child = graph.add_node(graph.get_root(), SAMPLE_LABEL);
  graph.set_accept(child, true);
  graph.remove_node(child);
  EXPECT_EQ(0, graph.get_num_accept());
}

TEST(LabeledGraph, RemoveNodeRecursive)
{
  const LabeledGraph empty_graph;
  LabeledGraph graph;
  auto child = graph.add_node(graph.get_root(), SAMPLE_LABEL);
  graph.add_node(child, SAMPLE_LABEL);
  EXPECT_EQ(2, graph.get_num_edges());
  graph.remove_node(child);
  EXPECT_EQ(1, graph.get_num_nodes());
  EXPECT_EQ(0, graph.get_num_edges());
  EXPECT_EQ(0, graph.get_num_accept());
  EXPECT_FALSE(graph.get_compacted());
  EXPECT_EQ(1, graph.get_nodes().size());
  EXPECT_TRUE(graph.get_source_counts().empty());
  EXPECT_TRUE(graph.get_dest_counts().empty());
  EXPECT_TRUE(graph.get_label_counts().empty());
  EXPECT_EQ(empty_graph, graph);
}

const auto SAMPLE_LABEL2 = "b";
const auto SAMPLE_LABEL3 = "c";

TEST(LabeledGraph, RemoveNodeParallel)
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

TEST(LabeledGraph, RemoveNodePreservesEqual)
{
  LabeledGraph graph1, graph2;
  auto node1 = graph1.add_node(graph1.get_root(), SAMPLE_LABEL);
  auto node2 = graph2.add_node(graph2.get_root(), SAMPLE_LABEL);
  graph1.remove_node(node1);
  graph2.remove_node(node2);
  EXPECT_EQ(graph1, graph2);
}

TEST(LabeledGraph, AddEdgeWithoutDest)
{
  LabeledGraph graph;
  graph.add_edge(graph.get_root(), SAMPLE_LABEL);
  EXPECT_EQ(2, graph.get_num_nodes());
  EXPECT_EQ(1, graph.get_num_edges());
}

// TODO: tests adding an edge with a source node not in the graph

TEST(LabeledGraph, AddEdgeWithoutDestPreservesEqual)
{
  LabeledGraph graph1, graph2;
  graph1.add_edge(graph1.get_root(), SAMPLE_LABEL);
  graph2.add_edge(graph2.get_root(), SAMPLE_LABEL);
  EXPECT_EQ(graph1, graph2);
}

TEST(LabeledGraph, RemoveEdge)
{
  LabeledGraph graph;
  graph.add_edge(graph.get_root(), SAMPLE_LABEL);
  graph.remove_edge(graph.get_root(), SAMPLE_LABEL);
  EXPECT_EQ(0, graph.get_num_edges());
}

// TODO: tests removing edge with source node not in the graph
// TODO: tests removing edge with label not in out-edge labels
// TODO: tests removing edge to child with in-degree 1

// TODO: tests iterating over nodes
