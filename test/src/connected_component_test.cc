//
// Created by smaptas on 25.10.17.
//

#include <memory>
#include <set>
#include <vector>

#include "../../src/cpp/graph/connected_component.h"
#include "../../src/cpp/graph/node.h"

#include "gtest/gtest.h"
#include "../../src/cpp/graph/graph.h"

class ConnectedComponentTest: public testing::Test
{
 protected:
  virtual void SetUp()
  {
    for (auto i = 0; i < 6; ++i) {
      nodes_.push_back(std::make_shared<Node>());
    }
    add_edge(nodes_[0], nodes_[1], a);
    add_edge(nodes_[0], nodes_[2], b);
    add_edge(nodes_[1], nodes_[3], a);
    add_edge(nodes_[3], nodes_[5], b);
    add_edge(nodes_[2], nodes_[4], a);
    add_edge(nodes_[4], nodes_[5], b);
    for (auto i = 1; i < 5; ++ i) {
      set_.insert(nodes_[i]);
    }
  }

  void add_edge(const std::shared_ptr<Node>& source,
                const std::shared_ptr<Node>& target,
                const std::string& label)
  {
    source->add_out_edge(label, target);
    target->add_in_edge(label, std::weak_ptr<Node>{source});
  }

  const std::string a = "a", b = "b";
  std::vector<std::shared_ptr<Node>> nodes_;
  std::set<std::shared_ptr<Node>> set_;
};

TEST_F(ConnectedComponentTest, CreateComponents)
{
  auto components = make_connected_components(set_);
  EXPECT_EQ(2, components.size());
  for (auto i = 0; i < 2; ++i) {
    EXPECT_EQ(2, components[i].size());
    EXPECT_EQ(1, components[i].upstream_size());
    EXPECT_TRUE(components[i].has_upstream_node(nodes_[0]));
    EXPECT_EQ(1, components[i].downstream_size());
    EXPECT_TRUE(components[i].has_downstream_node(nodes_[5]));
  }
  EXPECT_TRUE(components[0].has_node(nodes_[1])
              || components[1].has_node(nodes_[1]));
  if (components[0].has_node(nodes_[1])) {
    EXPECT_TRUE(components[0].has_node(nodes_[3])
                && components[1].has_node(nodes_[2])
                && components[1].has_node(nodes_[4]));
  } else {
    EXPECT_TRUE(components[1].has_node(nodes_[3])
                && components[0].has_node(nodes_[2])
                && components[0].has_node(nodes_[4]));
  }
}

TEST(ConnectedComponent, MakeConnectedComponents)
{
  LabeledGraph g;
  auto node01 = g.add_node(g.get_root(), "a");
  auto node02 = g.add_node(g.get_root(), "d");
  auto node03 = g.add_node(g.get_root(), "f");
  auto node04 = g.add_node(g.get_root(), "j");
  auto node05 = g.add_node(g.get_root(), "m");
  auto node06 = g.add_node(g.get_root(), "n");
  auto node07 = g.add_node(g.get_root(), "o");
  auto node08 = g.add_node(g.get_root(), "s");
  auto node09 = g.add_node(node01, "p");
  auto node10 = g.add_node(node01, "u");
  auto node11 = g.add_node(node09, "r");
  g.add_edge(node10, node11, "g");
  auto node12 = g.add_node(node02, "e");
  g.add_edge(node12, node11, "c");
  auto node13 = g.add_node(node03, "e");
  g.add_edge(node13, node11, "b");
  auto node14 = g.add_node(node04, "a");
  g.add_edge(node14, node11, "n");
  auto node15 = g.add_node(node04, "u");
  g.add_edge(node15, node11, "l");
  g.add_edge(node15, node11, "n");
  auto node16 = g.add_node(node05, "a");
  g.add_edge(node16, node11, "r");
  g.add_edge(node16, node11, "y");
  auto node17 = g.add_node(node06, "o");
  g.add_edge(node17, node11, "v");
  auto node18 = g.add_node(node07, "c");
  g.add_edge(node18, node11, "t");
  auto node19 = g.add_node(node08, "e");
  g.add_edge(node19, node11, "p");

  std::cerr << "node00: " << g.get_root() << std::endl;
  std::cerr << "node01: " << node01 << std::endl;
  std::cerr << "node02: " << node02 << std::endl;
  std::cerr << "node03: " << node03 << std::endl;
  std::cerr << "node04: " << node04 << std::endl;
  std::cerr << "node05: " << node05 << std::endl;
  std::cerr << "node06: " << node06 << std::endl;
  std::cerr << "node07: " << node07 << std::endl;
  std::cerr << "node08: " << node08 << std::endl;
  std::cerr << "node09: " << node09 << std::endl;
  std::cerr << "node10: " << node10 << std::endl;
  std::cerr << "node11: " << node11 << std::endl;
  std::cerr << "node12: " << node12 << std::endl;
  std::cerr << "node13: " << node13 << std::endl;
  std::cerr << "node14: " << node14 << std::endl;
  std::cerr << "node15: " << node15 << std::endl;
  std::cerr << "node16: " << node16 << std::endl;
  std::cerr << "node17: " << node17 << std::endl;
  std::cerr << "node18: " << node18 << std::endl;
  std::cerr << "node19: " << node19 << std::endl;

  std::set<std::shared_ptr<Node>> compaction_nodes{node02, node03, node05,
                                                   node06, node07, node08,
                                                   node09, node10, node12,
                                                   node13, node14, node17,
                                                   node18, node19};

  auto components = make_connected_components(compaction_nodes);
  EXPECT_EQ(9, components.size());
  auto one_node_components = 0, two_node_components = 0;
  for (auto comp: components) {
    EXPECT_GT(comp.size(), 0);
    EXPECT_LE(comp.size(), 2);
    EXPECT_EQ(comp.downstream_size(), 1);
    EXPECT_EQ(comp.upstream_size(), 1);
    if (comp.size() == 1) {
      ++one_node_components;
    } else if (comp.size() == 2) {
      ++two_node_components;
    }
    for (auto path: get_transitive_paths(comp)) {
      std::cerr << std::get<0>(path) << " " << std::get<1>(path) << " "
                << std::get<2>(path) << std::endl;
    }
  }
}
