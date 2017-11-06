/**
 * Author: smaptas
 */

// Include header file.

// Include C standard libraries.

// Include C++ standard libraries.

// Include other headers from this project.
#include "../../src/cpp/lexicon/fsa_lexicon.h"

// Include header from other projects.
#include "gtest/gtest.h"

TEST(FSALexicon, DefaultConstructor)
{
  FSALexicon lexicon;
  ASSERT_EQ(lexicon.size(), 0);
}

TEST(FSALexicon, AddString)
{
  FSALexicon lexicon;
  lexicon.add_string("abc");
  ASSERT_EQ(lexicon.size(), 1);
  ASSERT_TRUE(lexicon.has_string("abc"));
  auto dump = lexicon.dump_strings();
  ASSERT_FALSE(dump.find("abc") == dump.end());
//  EXPECT_EQ(2, lexicon.compact().size());
}

TEST(FSALexicon, Compact)
{
  FSALexicon lexicon;
  lexicon.add_string("abdcb");
  lexicon.add_string("abfhgb");
  lexicon.add_string("acgb");
  lexicon.add_string("bac");
  lexicon.add_string("baeb");
  lexicon.add_string("bdcb");
  lexicon.finalize();
  lexicon.compact();
//  std::cerr << lexicon.debug();
//  for (auto node: set) {
//    std::cerr << node << ": " << node->get_in_degree() << " " << node->get_out_degree() << std::endl;
//  }
//  EXPECT_EQ(8, set.size());
}

TEST(FSALexicon, Compact2)
{
  FSALexicon lexicon;
  lexicon.add_string("ca.google.www");
  lexicon.add_string("ch.google.mail");
  lexicon.add_string("ch.google.www");
  lexicon.add_string("com.google.mail");
  lexicon.add_string("com.google.www");
  lexicon.add_string("uk.co.google.www");
  lexicon.finalize();
  lexicon.compact();
//  EXPECT_EQ(29, lexicon.compact().size());
}

class NodeHashTest: public testing::Test
{
 protected:
  virtual void SetUp()
  {
    node1_ = std::make_shared<Node>();
    node2_ = std::make_shared<Node>();
    node3_ = std::make_shared<Node>();
    node4_ = std::make_shared<Node>();
  }

  std::shared_ptr<Node> node1_, node2_, node3_, node4_;
  const std::string label1_ = "a", label2_ = "b";
  NodeHash hash_;
};

TEST_F(NodeHashTest, EmptyNodes)
{
  EXPECT_EQ(hash_(node1_), hash_(node2_));
}

TEST_F(NodeHashTest, AcceptState)
{
  node1_->set_accept(true);
  EXPECT_NE(hash_(node1_), hash_(node2_));
  node2_->set_accept(true);
  EXPECT_EQ(hash_(node1_), hash_(node2_));
}

TEST_F(NodeHashTest, SameOutEdges)
{
  node1_->add_out_edge(label1_, node3_);
  EXPECT_NE(hash_(node1_), hash_(node2_));
  node2_->add_out_edge(label1_, node3_);
  EXPECT_EQ(hash_(node1_), hash_(node2_));
}

TEST_F(NodeHashTest, DifferentOutLabels)
{
  node1_->add_out_edge(label2_, node3_);
  node2_->add_out_edge(label1_, node3_);
  EXPECT_NE(hash_(node1_), hash_(node2_));
}

TEST_F(NodeHashTest, DifferentOutTargets)
{
  node1_->add_out_edge(label1_, node3_);
  node2_->add_out_edge(label1_, node4_);
  EXPECT_NE(hash_(node1_), hash_(node2_));
}

TEST_F(NodeHashTest, DifferentInSources)
{
  node1_->add_out_edge(label1_, node3_);
  node2_->add_out_edge(label1_, node3_);
  node1_->add_in_edge(label1_, std::weak_ptr<Node>(node3_));
  node2_->add_in_edge(label1_, std::weak_ptr<Node>(node4_));
  EXPECT_EQ(hash_(node1_), hash_(node2_));
}

TEST_F(NodeHashTest, DifferentInLabels)
{
  node1_->add_out_edge(label1_, node3_);
  node2_->add_out_edge(label1_, node3_);
  node1_->add_in_edge(label1_, std::weak_ptr<Node>(node4_));
  node2_->add_in_edge(label2_, std::weak_ptr<Node>(node4_));
  EXPECT_EQ(hash_(node1_), hash_(node2_));
}

class NodePredTest: public testing::Test
{
 protected:

  virtual void SetUp()
  {
    node1_ = std::make_shared<Node>();
    node2_ = std::make_shared<Node>();
    node3_ = std::make_shared<Node>();
    node4_ = std::make_shared<Node>();
  }

  std::shared_ptr<Node> node1_, node2_, node3_, node4_;
  const std::string label1_ = "a", label2_ = "b";
  NodePred pred_;
};

TEST_F(NodePredTest, EmptyNodes)
{
  EXPECT_TRUE(pred_(node1_, node2_));
}

TEST_F(NodePredTest, AcceptState)
{
  node1_->set_accept(true);
  EXPECT_FALSE(pred_(node1_, node2_));
  node2_->set_accept(true);
  EXPECT_TRUE(pred_(node1_, node2_));
}

TEST_F(NodePredTest, SameOutEdges)
{
  node1_->add_out_edge(label1_, node3_);
  EXPECT_FALSE(pred_(node1_, node2_));
  node2_->add_out_edge(label1_, node3_);
  EXPECT_TRUE(pred_(node1_, node2_));
}

TEST_F(NodePredTest, DifferentOutLabels)
{
  node1_->add_out_edge(label2_, node3_);
  node2_->add_out_edge(label1_, node3_);
  EXPECT_FALSE(pred_(node1_, node2_));
}

TEST_F(NodePredTest, DifferentOutTargets)
{
  node1_->add_out_edge(label1_, node3_);
  node2_->add_out_edge(label1_, node4_);
  EXPECT_FALSE(pred_(node1_, node2_));
}

TEST_F(NodePredTest, DifferentInSources)
{
  node1_->add_out_edge(label1_, node3_);
  node2_->add_out_edge(label1_, node3_);
  node1_->add_in_edge(label1_, std::weak_ptr<Node>(node3_));
  node2_->add_in_edge(label1_, std::weak_ptr<Node>(node4_));
  EXPECT_TRUE(pred_(node1_, node2_));
}

TEST_F(NodePredTest, DifferentInLabels)
{
  node1_->add_out_edge(label1_, node3_);
  node2_->add_out_edge(label1_, node3_);
  node1_->add_in_edge(label1_, std::weak_ptr<Node>(node4_));
  node2_->add_in_edge(label2_, std::weak_ptr<Node>(node4_));
  EXPECT_TRUE(pred_(node1_, node2_));
}
