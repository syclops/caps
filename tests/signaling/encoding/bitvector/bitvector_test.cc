//
// Created by smaptas on 09/24/18.
//

// Include other headers from this project.
#include "../../../../src/signaling/encoding/bitvector/bitvector.h"

// Include headers from other projects.
#include "gtest/gtest.h"
#include <boost/dynamic_bitset.hpp>

template <typename ImplType>
class BitVectorTest: public testing::Test
{
 protected:

  BitVector<ImplType> bv_;
};

template <typename ImplType>
class BitVectorComparisonTest: public BitVectorTest<ImplType>
{
 protected:
  BitVector<ImplType> bv2_;
};

using testing::Types;

// Test with a vector<bool>-based implementation
using Implementations = Types<std::vector<bool>, boost::dynamic_bitset<>>;

TYPED_TEST_CASE(BitVectorTest, Implementations);

TYPED_TEST(BitVectorTest, DefaultIsEmpty)
{
  EXPECT_TRUE(this->bv_.empty());
  EXPECT_EQ(0, this->bv_.size());
}

TYPED_TEST(BitVectorTest, WriteTrue)
{
  this->bv_.push_back(true);
  EXPECT_EQ(1, this->bv_.size());
  EXPECT_TRUE(this->bv_[0]);
}

TYPED_TEST(BitVectorTest, WriteFalse)
{
  this->bv_.push_back(false);
  EXPECT_EQ(1, this->bv_.size());
  EXPECT_FALSE(this->bv_[0]);
}

TYPED_TEST(BitVectorTest, WriteChar)
{
  const char test_char = 'a';
  this->bv_.push_back(test_char);
  EXPECT_EQ(BITS_IN_CHAR, this->bv_.size());
  std::ostringstream oss;
  oss << this->bv_;
  auto str = oss.str();
  EXPECT_EQ(1, str.size());
  EXPECT_EQ(test_char, str.at(0));
}

TYPED_TEST(BitVectorTest, WriteString)
{
  const std::string test_str = "foo";
  this->bv_.push_back(test_str);
  EXPECT_EQ(BITS_IN_CHAR * test_str.size(), this->bv_.size());
  std::ostringstream oss;
  oss << this->bv_;
  auto str = oss.str();
  EXPECT_EQ(test_str, str);
}

TYPED_TEST(BitVectorTest, WriteEmptyBitVectorToEmpty)
{
  BitVector<TypeParam> bv2;
  this->bv_.push_back(bv2);
  EXPECT_TRUE(this->bv_.empty());
}

TYPED_TEST(BitVectorTest, WriteEmptyBitVectorToNonempty)
{
  this->bv_.push_back(true);
  BitVector<TypeParam> copy(this->bv_);
  BitVector<TypeParam> empty_bv;
  this->bv_.push_back(empty_bv);
  EXPECT_EQ(copy, this->bv_);
}

// TODO: write nonempty to empty and nonempty to nonempty

// TODO: test pop_back

// TODO: test flip

TYPED_TEST_CASE(BitVectorComparisonTest, Implementations);

TYPED_TEST(BitVectorComparisonTest, DefaultIsEqual)
{
  EXPECT_EQ(this->bv_, this->bv2_);
}

TYPED_TEST(BitVectorComparisonTest, WriteToOneIsUnequal)
{
  this->bv_.push_back(true);
  EXPECT_NE(this->bv_, this->bv2_);
}

TYPED_TEST(BitVectorComparisonTest, WriteTrueIsEqual)
{
  bool b = true;
  this->bv_.push_back(b);
  this->bv2_.push_back(b);
  EXPECT_EQ(this->bv_, this->bv2_);
}

TYPED_TEST(BitVectorComparisonTest, WriteFalseIsEqual)
{
  bool b = false;
  this->bv_.push_back(b);
  this->bv2_.push_back(b);
  EXPECT_EQ(this->bv_, this->bv2_);
}

TYPED_TEST(BitVectorComparisonTest, WriteUnequalBitsIsUnequal)
{
  bool b = false;
  this->bv_.push_back(b);
  this->bv2_.push_back(!b);
  EXPECT_NE(this->bv_, this->bv2_);
}

TYPED_TEST(BitVectorComparisonTest, WriteCharIsEqual)
{
  char c = 'a';
  this->bv_.push_back(c);
  this->bv2_.push_back(c);
  EXPECT_EQ(this->bv_, this->bv2_);
}

TYPED_TEST(BitVectorComparisonTest, WriteUnequalCharIsUnequal)
{
  char c = 'a';
  char c2 = 'b';
  this->bv_.push_back(c);
  this->bv2_.push_back(c2);
  EXPECT_NE(this->bv_, this->bv2_);
}

TYPED_TEST(BitVectorComparisonTest, WriteStringIsEqual)
{
  std::string s = "foo";
  this->bv_.push_back(s);
  this->bv2_.push_back(s);
  EXPECT_EQ(this->bv_, this->bv2_);
}

TYPED_TEST(BitVectorComparisonTest, WriteUnequalStringIsUnequal)
{
  std::string s = "foo";
  std::string s2 = "bar";
  this->bv_.push_back(s);
  this->bv2_.push_back(s2);
  EXPECT_NE(this->bv_, this->bv2_);
}

TYPED_TEST(BitVectorComparisonTest, ClearIsDefault)
{
  std::string s = "foo";
  this->bv_.push_back(s);
  EXPECT_NE(this->bv_, this->bv2_);
  this->bv_.clear();
  EXPECT_EQ(this->bv_, this->bv2_);
}

