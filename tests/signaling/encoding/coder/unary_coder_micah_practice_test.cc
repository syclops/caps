//
// Created by micah on 9/28/21.
//

// Include C++ standard libraries.
#include <sstream>
#include <string>
#include <unordered_map>

// Include other headers from this project.
#include "../../../../src/signaling/common/contains.h"
#include "../../../src/signaling/encoding/coder/unary_coder_micah_practice.h"
#include "../../../../src/signaling/encoding/bitvector/bitvector.h"

// Include header from other projects.
#include "gtest/gtest.h"

//UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>> e1_;
template <typename IntType, typename BitVectorType>

class UnaryCoderMicahPracticeTest : public ::testing::Test {
//protected:
//    UnaryCoderMicahPractice<IntType, BitVectorType> e1_;

//    virtual void SetUp() override {
////        e1_.encode_impl("1");
////        e2_.encode_impl(2);
////        e3_.encode_impl(3);
//    }
};
TEST(UnaryCoderMicahPractice, CanTest) {
    UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>* e2_;

    e2_ = new UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>();
    BitVector<std::vector<bool>>* encoding = e2_->encode(1);
    auto decoded = e2_->decode(*encoding);

    EXPECT_EQ(std::get<0>(*decoded), 1);
};

TEST(UnaryCoderMicahPractice, CanTest2) {
UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>* e2_;

e2_ = new UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>();
BitVector<std::vector<bool>>* encoding = e2_->encode(2);
auto decoded = e2_->decode(*encoding);

EXPECT_EQ(std::get<0>(*decoded), 2);
};


TEST(UnaryCoderMicahPractice, IsDuplicableEncoding) {
UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>* e1_;
UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>* e2_;

e1_ = new UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>();
BitVector<std::vector<bool>>* encoding1 = e1_->encode(1);
e2_ = new UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>();
BitVector<std::vector<bool>>* encoding2 = e2_->encode(1);
EXPECT_EQ(*encoding1, *encoding2);
};

TEST(UnaryCoderMicahPractice, IsUnary) {
UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>* e1_;
e1_ = new UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>();
BitVector<std::vector<bool>>* encoding1 = e1_->encode(3);
BitVector<std::vector<bool>>* tester = new BitVector<std::vector<bool>>();
tester->push_back(1);
tester->push_back(1);
tester->push_back(1);
tester->push_back(0);
EXPECT_EQ(*encoding1, *tester);
};

TEST(UnaryCoderMicahPractice, IsDuplicableDecoding) {
UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>* e1_;
UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>* e2_;

e1_ = new UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>();
BitVector<std::vector<bool>>* encoding1 = e1_->encode(1);
e2_ = new UnaryCoderMicahPractice<int, BitVector<std::vector<bool>>>();
BitVector<std::vector<bool>>* encoding2 = e2_->encode(1);
auto decoding1 = e1_->decode(*encoding1);
auto decoding2 = e2_->decode(*encoding2);

EXPECT_EQ(decoding1, decoding2);
};
