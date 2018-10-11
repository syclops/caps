/**
 * Bitstream class unit tests
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

// Include other headers from this project.
#include "../../../src/signaling/encoding/bitstream.h"

// Include headers from other projects.
#include "gtest/gtest.h"

// Test fixture for adding data types to a Bitstream
class BitStreamTest: public testing::Test {
 protected:

  BitStream bstream_;
};

TEST_F(BitStreamTest, DefaultConstructor) {
  EXPECT_EQ(0, bstream_.size());
  EXPECT_TRUE(bstream_.empty());
}

// TODO: writing something into the bitstream should give us the same thing
// back.

// TODO: writing an int into the bitstream should only add the number of its
// bits in binary to the stream.

// TODO: padding should only change the size if its size is not a multiple of 8

// TODO: padding should always end in a size that's a multiple of 8

// TODO: equality checks

// TODO: ostream write tests