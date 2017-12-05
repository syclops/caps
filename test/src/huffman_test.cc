/**
 * Author: smaptas
 */

// Include header file.

// Include C standard libraries.

// Include C++ standard libraries.
#include <sstream>
#include <string>
#include <unordered_map>

// Include other headers from this project.
#include "../../src/cpp/encoding/huffman.h"

// Include header from other projects.
#include "gtest/gtest.h"
#include "../../src/cpp/common/contains.h"

class HuffmanCoderString: public testing::TestWithParam<std::string>
{
 public:
  virtual void SetUp()
  {
    string_ = GetParam();
    for (auto c: string_) {
      if (!contains(counts_, c)) {
        counts_.emplace(c, 0);
      }
      ++counts_[c];
    }
//    for (auto s: counts_) {
//      std::cout << s.first << " " << s.second << std::endl;
//    }
    coder_ = new HuffmanCoder<char>(counts_);
  }

  virtual void TearDown()
  {
    delete coder_;
  }

 protected:
  std::string string_;
  std::unordered_map<char, int> counts_;
  HuffmanCoder<char>* coder_;
};

TEST_P(HuffmanCoderString, EncodeDecodeInverse)
{
  for (auto itr: counts_) {
    EXPECT_EQ(std::string(1, itr.first), coder_->decode(coder_->encode(std::string(1, itr.first))));
  }
  EXPECT_EQ(string_, coder_->decode(coder_->encode(string_)));
}

const std::string kAbracadabra = "abracadabra";
const std::string kAbcdef = "aaaaabbbbbbbbbccccccccccccdddddddddddddeeeeeeeeeeeeeeeefffffffffffffffffffffffffffffffffffffffffffff";
const std::string kHellogoodbye = "hello, hello, i don't know why you say goodbye, i say hello.";
//const std::string kHellogoodbye = "hello";

INSTANTIATE_TEST_CASE_P(TestHuffmanString, HuffmanCoderString,
                        testing::Values(kAbracadabra, kAbcdef, kHellogoodbye));

TEST(HuffmanCoder, Abracadabra)
{
  std::unordered_map<char, int> counts;
  counts['a'] = 5;
  counts['b'] = 2;
  counts['r'] = 2;
  counts['c'] = 1;
  counts['d'] = 1;
  HuffmanCoder<char> coder(counts);
  EXPECT_EQ(coder.encode("a"), std::vector<bool>{0});
  EXPECT_EQ(coder.encode("b"), std::vector<bool>({1, 0, 0}));
  EXPECT_EQ(coder.encode("c"), std::vector<bool>({1, 0, 1}));
  EXPECT_EQ(coder.encode("d"), std::vector<bool>({1, 1, 0}));
  EXPECT_EQ(coder.encode("r"), std::vector<bool>({1, 1, 1}));
  EXPECT_EQ(coder.decode(std::vector<bool>{0}), "a");
  EXPECT_EQ(coder.decode(std::vector<bool>{1, 0, 0}), "b");
  EXPECT_EQ(coder.decode(std::vector<bool>{1, 0, 1}), "c");
  EXPECT_EQ(coder.decode(std::vector<bool>{1, 1, 0}), "d");
  EXPECT_EQ(coder.decode(std::vector<bool>{1, 1, 1}), "r");
//  EXPECT_EQ(coder.encode("b"), std::vector<bool>({1, 0, 0}));
//  EXPECT_EQ(coder.encode("c"), std::vector<bool>({1, 0, 1}));
//  EXPECT_EQ(coder.encode("d"), std::vector<bool>({1, 1, 0}));
//  EXPECT_EQ(coder.encode("r"), std::vector<bool>({1, 1, 1}));
  EXPECT_EQ("abracadabra", coder.decode(coder.encode("abracadabra")));
  EXPECT_EQ(coder.decode(std::vector<bool>({0, 1, 0, 0, 1, 1, 1})), "abr");
}

