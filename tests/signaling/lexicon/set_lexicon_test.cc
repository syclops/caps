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
#include "../../../src/signaling/lexicon/set_lexicon.h"

// Include header from other projects.
#include "gtest/gtest.h"

TEST(SetLexicon, DefaultConstructor)
{
  const SetLexicon lexicon;
  EXPECT_EQ(0, lexicon.size());
}

TEST(SetLexicon, CopyConstructor)
{
  const SetLexicon lexicon1;
  const auto lexicon2(lexicon1);
  EXPECT_EQ(0, lexicon2.size());
}

TEST(SetLexicon, EmptyEqual)
{
  const SetLexicon lexicon1, lexicon2;
  EXPECT_EQ(lexicon1, lexicon2);
}

const std::string TEST_STRING = "foo";

TEST(SetLexicon, CopyNonemptySetLexicon)
{
  SetLexicon lexicon1;
  lexicon1.add_string(TEST_STRING);
  const SetLexicon lexicon2(lexicon1);
  EXPECT_EQ(1, lexicon2.size());
  EXPECT_TRUE(lexicon2.has_string(TEST_STRING));
}

TEST(SetLexicon, AddString)
{
  SetLexicon lexicon;
  lexicon.add_string(TEST_STRING);
  EXPECT_EQ(1, lexicon.size());
}

TEST(SetLexicon, AddStringIdempotent)
{
  SetLexicon lexicon;
  lexicon.add_string(TEST_STRING);
  lexicon.add_string(TEST_STRING);
  EXPECT_EQ(1, lexicon.size());
}

TEST(SetLexicon, AddStringInSetLexicon)
{
  SetLexicon lexicon;
  lexicon.add_string(TEST_STRING);
  EXPECT_TRUE(lexicon.has_string(TEST_STRING));
}

TEST(SetLexicon, AddEqual)
{
  SetLexicon lexicon1, lexicon2;
  lexicon1.add_string(TEST_STRING);
  lexicon2.add_string(TEST_STRING);
  EXPECT_EQ(lexicon1, lexicon2);
}

const std::string TEST_STRING2 = "bar";

TEST(SetLexicon, AddCommutative)
{
  SetLexicon lexicon1, lexicon2;
  lexicon1.add_string(TEST_STRING);
  lexicon1.add_string(TEST_STRING2);
  lexicon2.add_string(TEST_STRING2);
  lexicon2.add_string(TEST_STRING);
  EXPECT_EQ(lexicon1, lexicon2);
}

TEST(SetLexicon, DumpLoadInverse)
{
  SetLexicon lexicon1, lexicon2;
  lexicon1.add_string(TEST_STRING);
  lexicon1.add_string(TEST_STRING2);
  std::set<std::string> strings = lexicon1.dump_strings();
  std::stringstream sstream;
  for (std::string str: strings) {
    sstream << str << "\n";
  }
  lexicon2.load(sstream);
  EXPECT_EQ(lexicon1, lexicon2);
}

class SetLexiconFileFixture: public testing::Test
{
 protected:

  virtual void SetUp()
  {
    strings_.insert(TEST_STRING);
    strings_.insert(TEST_STRING2);
    for (const auto& str: strings_) {
      sstream_ << str << "\n";
    }
  }

  std::stringstream sstream_;
  std::set<std::string> strings_;
};

TEST_F(SetLexiconFileFixture, LoadDumpInverse)
{
  SetLexicon lexicon;
  lexicon.load(sstream_);
  EXPECT_TRUE(std::equal(strings_.begin(), strings_.end(),
                         lexicon.dump_strings().begin()));
}
