/**
 * Author: smaptas
 */

// Include header file.

// Include C standard libraries.

// Include C++ standard libraries.
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// Include other headers from this project.
#include "../../../src/signaling/graph/node/node.h"
#include "../../../src/signaling/lexicon/fsa_lexicon/fsa_lexicon.h"

// Include header from other projects.
#include "gtest/gtest.h"


//class FSALexiconAdd: public testing::TestWithParam<std::set<std::string>>
//{
// public:
//
//  virtual void SetUp()
//  {
//    strings_ = GetParam();
//    std::stringstream stream;
//    for (auto str: strings_) {
//      stream << str << std::endl;
//    }
//    lexicon_.add_file(stream);
//  }
//
// protected:
//  std::set<std::string> strings_;
//  FSALexicon lexicon_;
//};
//
//TEST_P(FSALexiconAdd, AddsCorrectStrings)
//{
//  EXPECT_EQ(strings_.size(), lexicon_.size());
//  EXPECT_TRUE(std::equal(strings_.begin(), strings_.end(),
//                         lexicon_.dump_strings().begin()));
//}
//
//const std::set<std::string> kMonthsLong =  {"january", "february", "march",
//                                            "april", "may", "june", "july",
//                                            "august", "september", "october",
//                                            "november", "december"};
//
//const std::set<std::string> kMonthsAbbrev =  {"jan", "feb", "mar", "apr", "may",
//                                              "jun", "jul", "aug", "sep", "oct",
//                                              "nov", "dec"};
//
//const std::set<std::string> kGoogleDomains = {"ca.google.www", "ch.google.mail",
//                                              "ch.google.www", "com.google",
//                                              "com.google.mail",
//                                              "com.google.www",
//                                              "uk.co.google.www"};
//
//const std::set<std::string> kGoogleDomainsRepr = {"cbi", "cde", "cdhi", "cf",
//                                                  "cfae", "cfahi", "gi"};
//
//const std::set<std::string> kCompactionExample =  {"abdcb", "abfhgb", "acgb",
//                                                   "bac", "baeb", "bdcb"};
//
//TEST(FSALexiconCompact, Compaction)
//{
//  FSALexicon lexicon;
//  std::stringstream stream;
//  for (auto str: kMonthsLong) {
//    stream << str << std::endl;
//  }
//  lexicon.add_file(stream);
//  lexicon.compact();
//}
//
//
//INSTANTIATE_TEST_CASE_P(TestAddFile, FSALexiconAdd,
//                        testing::Values(kMonthsLong, kMonthsAbbrev,
//                                        kGoogleDomains, kGoogleDomainsRepr,
//                                        kCompactionExample));
//
//class FSALexiconCompact: public FSALexiconAdd
//{
// public:
//  virtual void SetUp()
//  {
//    FSALexiconAdd::SetUp();
//    lexicon_.compact_long_edges();
//  }
//};
//
//TEST_P(FSALexiconCompact, AddsCorrectStrings)
//{
//  EXPECT_EQ(strings_.size(), lexicon_.size());
//  EXPECT_TRUE(std::equal(strings_.begin(), strings_.end(),
//                         lexicon_.dump_strings().begin()));
//}
//
//INSTANTIATE_TEST_CASE_P(TestAddFile, FSALexiconCompact,
//                        testing::Values(kMonthsLong, kMonthsAbbrev,
//                                        kGoogleDomains, kGoogleDomainsRepr,
//                                        kCompactionExample));
//
//TEST(FSALexicon, DefaultConstructor)
//{
//  FSALexicon lexicon;
//  ASSERT_EQ(lexicon.size(), 0);
//}
//
//TEST(FSALexicon, AddString)
//{
//  FSALexicon lexicon;
//  lexicon.add_string("abc");
//  ASSERT_EQ(lexicon.size(), 1);
//  ASSERT_TRUE(lexicon.has_string("abc"));
//  auto dump = lexicon.dump_strings();
//  ASSERT_FALSE(dump.find("abc") == dump.end());
////  EXPECT_EQ(2, lexicon.compact().size());
//}


class GoogleLexiconShort: public testing::Test
{
 protected:

  virtual void add_n_strings(size_t n)
  {
    for (size_t i = 0; i < n; ++i) {
      lexicon_.add_string(domains_[i]);
    }
  }

  FSALexicon lexicon_;
  const std::vector<std::string> domains_ = {"cawx", "ch/m", "ch/wx", "co/m",
                                             "co/wx", "u", "u.wx"};
};

TEST_F(GoogleLexiconShort, AddString1)
{
  add_n_strings(1);
  EXPECT_EQ(5, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(4, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(1, lexicon_.get_graph().get_num_accept());
  EXPECT_TRUE(lexicon_.get_register().empty());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(1, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(1, label_map.at("w"));
  EXPECT_EQ(1, label_map.at("x"));
}

TEST_F(GoogleLexiconShort, AddString2)
{
  add_n_strings(2);
  EXPECT_EQ(8, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(7, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(2, lexicon_.get_graph().get_num_accept());
  EXPECT_EQ(3, lexicon_.get_register().size());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(1, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(1, label_map.at("m"));
  EXPECT_EQ(1, label_map.at("w"));
  EXPECT_EQ(1, label_map.at("x"));
  EXPECT_EQ(1, label_map.at("/"));
}

TEST_F(GoogleLexiconShort, AddString3)
{
  add_n_strings(3);
  EXPECT_EQ(9, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(9, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(2, lexicon_.get_graph().get_num_accept());
  EXPECT_EQ(3, lexicon_.get_register().size());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(1, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(1, label_map.at("m"));
  EXPECT_EQ(2, label_map.at("w"));
  EXPECT_EQ(2, label_map.at("x"));
  EXPECT_EQ(1, label_map.at("/"));
}

TEST_F(GoogleLexiconShort, AddString4)
{
  add_n_strings(4);
  EXPECT_EQ(10, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(11, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(2, lexicon_.get_graph().get_num_accept());
  EXPECT_EQ(5, lexicon_.get_register().size());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(1, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(2, label_map.at("m"));
  EXPECT_EQ(1, label_map.at("o"));
  EXPECT_EQ(2, label_map.at("w"));
  EXPECT_EQ(1, label_map.at("x"));
  EXPECT_EQ(2, label_map.at("/"));
}

TEST_F(GoogleLexiconShort, AddString5)
{
  add_n_strings(5);
  EXPECT_EQ(11, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(13, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(2, lexicon_.get_graph().get_num_accept());
  EXPECT_EQ(5, lexicon_.get_register().size());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(1, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(2, label_map.at("m"));
  EXPECT_EQ(1, label_map.at("o"));
  EXPECT_EQ(3, label_map.at("w"));
  EXPECT_EQ(2, label_map.at("x"));
  EXPECT_EQ(2, label_map.at("/"));
}

TEST_F(GoogleLexiconShort, AddString7)
{
  add_n_strings(7);
  EXPECT_EQ(8, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(11, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(2, lexicon_.get_graph().get_num_accept());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(1, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(1, label_map.at("m"));
  EXPECT_EQ(1, label_map.at("o"));
  EXPECT_EQ(1, label_map.at("u"));
  EXPECT_EQ(2, label_map.at("w"));
  EXPECT_EQ(1, label_map.at("x"));
  EXPECT_EQ(2, label_map.at("."));
}

class GoogleLexicon: public testing::Test
{
 protected:

  virtual void add_n_strings(size_t n)
  {
    for (size_t i = 0; i < n; ++i) {
      lexicon_.add_string(domains_[i]);
    }
  }

  FSALexicon lexicon_;
  const std::vector<std::string> domains_ = {"ca.google.www", "ch.google.mail",
                                             "ch.google.www", "com.google.mail",
                                             "com.google.www", "uk.co.google",
                                             "uk.co.google.www"};
};

TEST_F(GoogleLexicon, AddString1)
{
  add_n_strings(1);
  EXPECT_EQ(14, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(13, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(1, lexicon_.get_graph().get_num_accept());
  EXPECT_TRUE(lexicon_.get_register().empty());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(1, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(1, label_map.at("e"));
  EXPECT_EQ(2, label_map.at("g"));
  EXPECT_EQ(1, label_map.at("l"));
  EXPECT_EQ(2, label_map.at("o"));
  EXPECT_EQ(3, label_map.at("w"));
  EXPECT_EQ(2, label_map.at("."));
}

TEST_F(GoogleLexicon, AddString2)
{
  add_n_strings(2);
  EXPECT_EQ(27, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(26, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(2, lexicon_.get_graph().get_num_accept());
  EXPECT_EQ(12, lexicon_.get_register().size());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(2, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(2, label_map.at("e"));
  EXPECT_EQ(4, label_map.at("g"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(1, label_map.at("i"));
  EXPECT_EQ(3, label_map.at("l"));
  EXPECT_EQ(1, label_map.at("m"));
  EXPECT_EQ(4, label_map.at("o"));
  EXPECT_EQ(3, label_map.at("w"));
  EXPECT_EQ(4, label_map.at("."));
}

TEST_F(GoogleLexicon, AddString3)
{
  add_n_strings(3);
  EXPECT_EQ(29, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(29, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(2, lexicon_.get_graph().get_num_accept());
  EXPECT_EQ(15, lexicon_.get_register().size());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(2, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(2, label_map.at("e"));
  EXPECT_EQ(4, label_map.at("g"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(1, label_map.at("i"));
  EXPECT_EQ(3, label_map.at("l"));
  EXPECT_EQ(1, label_map.at("m"));
  EXPECT_EQ(4, label_map.at("o"));
  EXPECT_EQ(6, label_map.at("w"));
  EXPECT_EQ(4, label_map.at("."));
}

TEST_F(GoogleLexicon, AddString4)
{
  add_n_strings(4);
  EXPECT_EQ(40, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(41, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(2, lexicon_.get_graph().get_num_accept());
  EXPECT_EQ(24, lexicon_.get_register().size());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(3, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(3, label_map.at("e"));
  EXPECT_EQ(6, label_map.at("g"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(2, label_map.at("i"));
  EXPECT_EQ(5, label_map.at("l"));
  EXPECT_EQ(3, label_map.at("m"));
  EXPECT_EQ(7, label_map.at("o"));
  EXPECT_EQ(4, label_map.at("w"));
  EXPECT_EQ(6, label_map.at("."));
}

TEST_F(GoogleLexicon, AddString5)
{
  add_n_strings(5);
  EXPECT_EQ(39, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(41, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(2, lexicon_.get_graph().get_num_accept());
  EXPECT_EQ(24, lexicon_.get_register().size());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(2, label_map.at("a"));
  EXPECT_EQ(1, label_map.at("c"));
  EXPECT_EQ(3, label_map.at("e"));
  EXPECT_EQ(6, label_map.at("g"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(1, label_map.at("i"));
  EXPECT_EQ(4, label_map.at("l"));
  EXPECT_EQ(3, label_map.at("m"));
  EXPECT_EQ(7, label_map.at("o"));
  EXPECT_EQ(7, label_map.at("w"));
  EXPECT_EQ(6, label_map.at("."));
}

TEST_F(GoogleLexicon, AddString6)
{
  add_n_strings(6);
  EXPECT_EQ(39, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(41, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(2, lexicon_.get_graph().get_num_accept());
  EXPECT_EQ(26, lexicon_.get_register().size());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(2, label_map.at("a"));
  EXPECT_EQ(2, label_map.at("c"));
  EXPECT_EQ(3, label_map.at("e"));
  EXPECT_EQ(6, label_map.at("g"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(1, label_map.at("i"));
  EXPECT_EQ(1, label_map.at("k"));
  EXPECT_EQ(4, label_map.at("l"));
  EXPECT_EQ(2, label_map.at("m"));
  EXPECT_EQ(8, label_map.at("o"));
  EXPECT_EQ(1, label_map.at("u"));
  EXPECT_EQ(4, label_map.at("w"));
  EXPECT_EQ(6, label_map.at("."));
}

TEST_F(GoogleLexicon, AddString7)
{
  add_n_strings(7);
  EXPECT_EQ(43, lexicon_.get_graph().get_num_nodes());
  EXPECT_EQ(45, lexicon_.get_graph().get_num_edges());
  EXPECT_EQ(3, lexicon_.get_graph().get_num_accept());
  EXPECT_EQ(26, lexicon_.get_register().size());
  const auto& label_map = lexicon_.get_graph().get_label_counts();
  EXPECT_EQ(2, label_map.at("a"));
  EXPECT_EQ(2, label_map.at("c"));
  EXPECT_EQ(3, label_map.at("e"));
  EXPECT_EQ(6, label_map.at("g"));
  EXPECT_EQ(1, label_map.at("h"));
  EXPECT_EQ(1, label_map.at("i"));
  EXPECT_EQ(1, label_map.at("k"));
  EXPECT_EQ(4, label_map.at("l"));
  EXPECT_EQ(2, label_map.at("m"));
  EXPECT_EQ(8, label_map.at("o"));
  EXPECT_EQ(1, label_map.at("u"));
  EXPECT_EQ(7, label_map.at("w"));
  EXPECT_EQ(7, label_map.at("."));
}

//class NodeHashTest: public testing::Test
//{
// protected:
//  virtual void SetUp()
//  {
//    node1_ = std::make_shared<Node>();
//    node2_ = std::make_shared<Node>();
//    node3_ = std::make_shared<Node>();
//    node4_ = std::make_shared<Node>();
//  }
//
//  std::shared_ptr<Node> node1_, node2_, node3_, node4_;
//  const std::string label1_ = "a", label2_ = "b";
//  NodeHash hash_;
//};

//TEST_F(NodeHashTest, EmptyNodes)
//{
//  EXPECT_EQ(hash_(node1_), hash_(node2_));
//}
//
//TEST_F(NodeHashTest, AcceptState)
//{
//  node1_->set_accept(true);
//  EXPECT_NE(hash_(node1_), hash_(node2_));
//  node2_->set_accept(true);
//  EXPECT_EQ(hash_(node1_), hash_(node2_));
//}
//
//TEST_F(NodeHashTest, SameOutEdges)
//{
//  node1_->add_out_edge(label1_, node3_);
//  EXPECT_NE(hash_(node1_), hash_(node2_));
//  node2_->add_out_edge(label1_, node3_);
//  EXPECT_EQ(hash_(node1_), hash_(node2_));
//}
//
//TEST_F(NodeHashTest, DifferentOutLabels)
//{
//  node1_->add_out_edge(label2_, node3_);
//  node2_->add_out_edge(label1_, node3_);
//  EXPECT_NE(hash_(node1_), hash_(node2_));
//}
//
//TEST_F(NodeHashTest, DifferentOutTargets)
//{
//  node1_->add_out_edge(label1_, node3_);
//  node2_->add_out_edge(label1_, node4_);
//  EXPECT_NE(hash_(node1_), hash_(node2_));
//}
//
//TEST_F(NodeHashTest, DifferentInSources)
//{
//  node1_->add_out_edge(label1_, node3_);
//  node2_->add_out_edge(label1_, node3_);
//  node1_->add_in_edge(label1_, std::weak_ptr<Node>(node3_));
//  node2_->add_in_edge(label1_, std::weak_ptr<Node>(node4_));
//  EXPECT_EQ(hash_(node1_), hash_(node2_));
//}
//
//TEST_F(NodeHashTest, DifferentInLabels)
//{
//  node1_->add_out_edge(label1_, node3_);
//  node2_->add_out_edge(label1_, node3_);
//  node1_->add_in_edge(label1_, std::weak_ptr<Node>(node4_));
//  node2_->add_in_edge(label2_, std::weak_ptr<Node>(node4_));
//  EXPECT_EQ(hash_(node1_), hash_(node2_));
//}
//
//class NodePredTest: public testing::Test
//{
// protected:
//
//  virtual void SetUp()
//  {
//    node1_ = std::make_shared<Node>();
//    node2_ = std::make_shared<Node>();
//    node3_ = std::make_shared<Node>();
//    node4_ = std::make_shared<Node>();
//  }
//
//  std::shared_ptr<Node> node1_, node2_, node3_, node4_;
//  const std::string label1_ = "a", label2_ = "b";
//  NodePred pred_;
//};
//
//TEST_F(NodePredTest, EmptyNodes)
//{
//  EXPECT_TRUE(pred_(node1_, node2_));
//}
//
//TEST_F(NodePredTest, AcceptState)
//{
//  node1_->set_accept(true);
//  EXPECT_FALSE(pred_(node1_, node2_));
//  node2_->set_accept(true);
//  EXPECT_TRUE(pred_(node1_, node2_));
//}
//
//TEST_F(NodePredTest, SameOutEdges)
//{
//  node1_->add_out_edge(label1_, node3_);
//  EXPECT_FALSE(pred_(node1_, node2_));
//  node2_->add_out_edge(label1_, node3_);
//  EXPECT_TRUE(pred_(node1_, node2_));
//}
//
//TEST_F(NodePredTest, DifferentOutLabels)
//{
//  node1_->add_out_edge(label2_, node3_);
//  node2_->add_out_edge(label1_, node3_);
//  EXPECT_FALSE(pred_(node1_, node2_));
//}
//
//TEST_F(NodePredTest, DifferentOutTargets)
//{
//  node1_->add_out_edge(label1_, node3_);
//  node2_->add_out_edge(label1_, node4_);
//  EXPECT_FALSE(pred_(node1_, node2_));
//}
//
//TEST_F(NodePredTest, DifferentInSources)
//{
//  node1_->add_out_edge(label1_, node3_);
//  node2_->add_out_edge(label1_, node3_);
//  node1_->add_in_edge(label1_, std::weak_ptr<Node>(node3_));
//  node2_->add_in_edge(label1_, std::weak_ptr<Node>(node4_));
//  EXPECT_TRUE(pred_(node1_, node2_));
//}
//
//TEST_F(NodePredTest, DifferentInLabels)
//{
//  node1_->add_out_edge(label1_, node3_);
//  node2_->add_out_edge(label1_, node3_);
//  node1_->add_in_edge(label1_, std::weak_ptr<Node>(node4_));
//  node2_->add_in_edge(label2_, std::weak_ptr<Node>(node4_));
//  EXPECT_TRUE(pred_(node1_, node2_));
//}
