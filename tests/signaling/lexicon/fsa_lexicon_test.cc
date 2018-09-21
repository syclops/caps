/**
 * Author: smaptas
 */

// Include header file.

// Include C standard libraries.

// Include C++ standard libraries.
#include <set>
#include <sstream>
#include <string>

// Include other headers from this project.
#include "../../../src/signaling/graph/node.h"
#include "../../../src/signaling/lexicon/fsa_lexicon.h"

// Include header from other projects.
#include "gtest/gtest.h"


class FSALexiconAdd: public testing::TestWithParam<std::set<std::string>>
{
 public:

  virtual void SetUp()
  {
    strings_ = GetParam();
    std::stringstream stream;
    for (auto str: strings_) {
      stream << str << std::endl;
    }
    lexicon_.add_file(stream);
  }

 protected:
  std::set<std::string> strings_;
  FSALexicon lexicon_;
};

TEST_P(FSALexiconAdd, AddsCorrectStrings)
{
  EXPECT_EQ(strings_.size(), lexicon_.size());
  EXPECT_TRUE(std::equal(strings_.begin(), strings_.end(),
                         lexicon_.dump_strings().begin()));
}

const std::set<std::string> kMonthsLong =  {"january", "february", "march",
                                            "april", "may", "june", "july",
                                            "august", "september", "october",
                                            "november", "december"};

const std::set<std::string> kMonthsAbbrev =  {"jan", "feb", "mar", "apr", "may",
                                              "jun", "jul", "aug", "sep", "oct",
                                              "nov", "dec"};

const std::set<std::string> kGoogleDomains = {"ca.google.www", "ch.google.mail",
                                              "ch.google.www", "com.google",
                                              "com.google.mail",
                                              "com.google.www",
                                              "uk.co.google.www"};

const std::set<std::string> kGoogleDomainsRepr = {"cbi", "cde", "cdhi", "cf",
                                                  "cfae", "cfahi", "gi"};

const std::set<std::string> kCompactionExample =  {"abdcb", "abfhgb", "acgb",
                                                   "bac", "baeb", "bdcb"};

TEST(FSALexiconCompact, Compaction)
{
  FSALexicon lexicon;
  std::stringstream stream;
  for (auto str: kMonthsLong) {
    stream << str << std::endl;
  }
  lexicon.add_file(stream);
  lexicon.compact();
}


INSTANTIATE_TEST_CASE_P(TestAddFile, FSALexiconAdd,
                        testing::Values(kMonthsLong, kMonthsAbbrev,
                                        kGoogleDomains, kGoogleDomainsRepr,
                                        kCompactionExample));

class FSALexiconCompact: public FSALexiconAdd
{
 public:
  virtual void SetUp()
  {
    FSALexiconAdd::SetUp();
    lexicon_.compact_long_edges();
  }
};

TEST_P(FSALexiconCompact, AddsCorrectStrings)
{
  EXPECT_EQ(strings_.size(), lexicon_.size());
  EXPECT_TRUE(std::equal(strings_.begin(), strings_.end(),
                         lexicon_.dump_strings().begin()));
}

INSTANTIATE_TEST_CASE_P(TestAddFile, FSALexiconCompact,
                        testing::Values(kMonthsLong, kMonthsAbbrev,
                                        kGoogleDomains, kGoogleDomainsRepr,
                                        kCompactionExample));

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
