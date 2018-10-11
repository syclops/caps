/**
 * Author: smaptas
 */

// Include header file.
#include "bitstream.h"

// Include C standard libraries.
#include <cmath>
#include <cstdlib>

// Include C++ standard libraries.
#include <iostream>
#include <string>

// Include other headers from this project.

// Include header from other projects.

void BitStream::push_back(char c)
{
  reserve(size() + CHAR_SIZE);
  for (size_t i = CHAR_SIZE; i > 0; --i) {
    push_back(static_cast<bool>(c >> (i - 1) & 1));
  }
}

void BitStream::push_back(const std::string& s)
{
  reserve(size() + CHAR_SIZE * s.size());
  for (char c: s) {
    push_back(c);
  }
}

void BitStream::push_back(std::string&& s)
{
  reserve(size() + CHAR_SIZE * s.size());
  for (char c: s) {
    push_back(c);
  }
}

void BitStream::push_back(const std::vector<bool>& v)
{
  reserve(size() + v.size());
  for (auto b: v) {
    push_back(b);
  }
}

void BitStream::push_back(std::vector<bool>&& v)
{
  reserve(size() + v.size());
  std::move(v.begin(), v.end(), begin());
}

template<typename IntType>
void BitStream::push_back(IntType i)
{
  if (i == 0) {
    push_back(false);
  } else {
    auto num_bits = static_cast<size_t>(ceil(log2(i)));
    for (auto j = num_bits; j > 0; --j) {
      push_back(static_cast<bool>(i >> (j - 1) & 1));
    }
  }
}

void BitStream::pad(bool b)
{
  resize(CHAR_SIZE * static_cast<size_t>(ceil(static_cast<double>(
    size()) / CHAR_SIZE)), b);
}

bool operator==(const BitStream& lhs, const BitStream& rhs)
{
  if (lhs.size() != rhs.size()) {
    return false;
  }
  // TODO: make sure this works with vector<bool>'s non-standard iterators
  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

bool operator!=(const BitStream& lhs, const BitStream& rhs)
{
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out_stream, BitStream& bitStream)
{
  bitStream.pad();
  for (size_t i = 0; i < bitStream.size(); i += BitStream::CHAR_SIZE) {
    char c = 0;
    for (size_t j = 0; j < BitStream::CHAR_SIZE; ++j) {
      c = (c << 1) + bitStream[i + j];
    }
    out_stream << c;
  }
  return out_stream;
}

