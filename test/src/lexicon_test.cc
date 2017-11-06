/**
 * Author: smaptas
 */

// Include header file.

// Include C standard libraries.

// Include C++ standard libraries.
#include <sstream>

// Include other headers from this project.
#include "../../src/cpp/lexicon/lexicon.h"
#include "../../src/cpp/lexicon/set_lexicon.h"
#include "../../src/cpp/lexicon/fsa_lexicon.h"

// Include header from other projects.
#include "gtest/gtest.h"

template <typename LexiconType>
class LexiconTest: public testing::Test
{
 protected:
  LexiconTest()
    : lexicon_{new LexiconType}
  {
    // Nothing to do here.
  }

  virtual ~LexiconTest()
  {
    delete lexicon_;
  }

  LexiconType* lexicon_;
};

using testing::Types;
typedef Types<SetLexicon, FSALexicon> Implementations;

template <typename LexiconType>
class LexiconStringTest: public LexiconTest<LexiconType>
{
 protected:
  const std::string SAMPLE_URL = "com.example.www";
};

template <typename LexiconType>
class LexiconPrefixTest: public LexiconStringTest<LexiconType>
{
 protected:
  const std::string SAMPLE_PREFIX = "com.example";
};


TYPED_TEST_CASE(LexiconTest, Implementations);

TYPED_TEST(LexiconTest, CreateEmptyLexicon)
{
  EXPECT_EQ(0, this->lexicon_->size());
  EXPECT_TRUE(this->lexicon_->dump_strings().empty());
}

TYPED_TEST(LexiconTest, EmptyLexiconsEqual)
{
  TypeParam* empty_copy = new TypeParam{*(this->lexicon_)};
  EXPECT_EQ(*(this->lexicon_), *empty_copy);
  delete empty_copy;
}


TYPED_TEST_CASE(LexiconStringTest, Implementations);

TYPED_TEST(LexiconStringTest, AddString)
{
  this->lexicon_->add_string(this->SAMPLE_URL);
  EXPECT_EQ(1, this->lexicon_->size());
  EXPECT_FALSE(this->lexicon_->dump_strings().empty());
  EXPECT_TRUE(this->lexicon_->has_string(this->SAMPLE_URL));
}

TYPED_TEST(LexiconStringTest, AddStringIdempotent)
{
  this->lexicon_->add_string(this->SAMPLE_URL);
  this->lexicon_->add_string(this->SAMPLE_URL);
  EXPECT_EQ(1, this->lexicon_->size());
  EXPECT_TRUE(this->lexicon_->has_string(this->SAMPLE_URL));
}

TYPED_TEST(LexiconStringTest, AddStringPreservesEqual)
{
  TypeParam* copy = new TypeParam{*(this->lexicon_)};
  this->lexicon_->add_string(this->SAMPLE_URL);
  ASSERT_NE(*(this->lexicon_), *copy);
  copy->add_string(this->SAMPLE_URL);
  EXPECT_EQ(*(this->lexicon_), *copy);
}


TYPED_TEST_CASE(LexiconPrefixTest, Implementations);

TYPED_TEST(LexiconPrefixTest, AddString)
{
  this->lexicon_->add_string(this->SAMPLE_URL);
  EXPECT_FALSE(this->lexicon_->has_string(this->SAMPLE_PREFIX));
}

TYPED_TEST(LexiconPrefixTest, AddPrefix)
{
  this->lexicon_->add_string(this->SAMPLE_PREFIX);
  this->lexicon_->add_string(this->SAMPLE_URL);
  EXPECT_TRUE(this->lexicon_->has_string(this->SAMPLE_PREFIX));
}