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

TYPED_TEST_SUITE(BitVectorTest, Implementations);

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

TYPED_TEST(BitVectorTest, PopBack)
{
this->bv_.push_back(false);
this->bv_.push_back(true);
EXPECT_EQ(2, this->bv_.size());
EXPECT_FALSE(this->bv_[0]);
EXPECT_TRUE(this->bv_[1]);
this->bv_.pop_back();
EXPECT_EQ(1, this->bv_.size());
EXPECT_FALSE(this->bv_[0]);
}

// TODO: test flip
TYPED_TEST(BitVectorTest, Flip)
{
this->bv_.push_back(false);
this->bv_.push_back(true);
this->bv_.push_back(false);
EXPECT_EQ(3, this->bv_.size());
EXPECT_FALSE(this->bv_[0]);
EXPECT_TRUE(this->bv_[1]);
EXPECT_FALSE(this->bv_[0]);
this->bv_.flip();
EXPECT_EQ(3, this->bv_.size());
EXPECT_TRUE(this->bv_[0]);
EXPECT_FALSE(this->bv_[1]);
EXPECT_TRUE(this->bv_[0]);
}
TYPED_TEST_SUITE(BitVectorComparisonTest, Implementations);

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

