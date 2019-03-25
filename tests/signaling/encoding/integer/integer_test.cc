/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

// Include C standard libraries.

// Include C++ standard libraries.
#include <vector>

// Include other headers from this project.
#include "../../../../src/signaling/encoding/integer.h"
#include "../../../../src/signaling/encoding/bitvector/bitvector.h"

// Include headers from other projects.
#include "gtest/gtest.h"

// test value pairs

// check encoding and decoding works

// also check encode/decode and decode/encode works

// check decode off the front

// TODO: replace the tests below with a more parameterized set of tests.

class UnaryCoderTest: public testing::Test
{
 public:

  using IntType = int;
  using BitVectorType = BitVector<std::vector<bool>>;

 protected:

  UnaryCoder<IntType, BitVectorType> coder_;

};

TEST_F(UnaryCoderTest, EncodeZero)
{
  auto encoding = coder_.encode(0);
  EXPECT_EQ(1, encoding.size());
  EXPECT_TRUE(encoding[0]);
}

TEST_F(UnaryCoderTest, EncodeZeroFalseTerm)
{
  auto encoding = coder_.encode(0);
  EXPECT_EQ(1, encoding.size());
  EXPECT_FALSE(encoding[0]);
}

TEST_F(UnaryCoderTest, EncodeOne)
{
  auto encoding = coder_.encode(1);
  EXPECT_EQ(2, encoding.size());
  EXPECT_FALSE(encoding[0]);
  EXPECT_TRUE(encoding[1]);
}
