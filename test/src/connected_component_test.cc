//
// Created by smaptas on 25.10.17.
//

#include <memory>
#include <set>
#include <vector>

#include "../../src/cpp/graph/connected_component.h"
#include "../../src/cpp/graph/node.h"

#include "gtest/gtest.h"

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
