/**
 * Author: smaptas
 */

// Include header file.
#include "bitstream.h"

// Include C standard libraries.
#include <cstdlib>

// Include C++ standard libraries.
#include <iostream>
#include <string>
#include <cmath>

// Include other headers from this project.

// Include header from other projects.

BitStream& BitStream::operator<<(bool b)
{
  bits_.push_back(b);
  return *this;
}

BitStream& BitStream::operator<<(char c)
{
  bits_.reserve(bits_.size() + CHAR_SIZE);
  for (size_t i = CHAR_SIZE; i > 0; --i) {
    bits_.push_back((bool) (c >> (i - 1) & 1));
  }
  return *this;
}

BitStream& BitStream::operator<<(const std::string& s)
{
  bits_.reserve(bits_.size() + CHAR_SIZE * s.size());
  for (char c: s) {
    *this << c;
  }
  return *this;
}

BitStream& BitStream::operator<<(const std::vector<bool>& v)
{
  bits_.reserve(bits_.size() + v.size());
  for (bool bit: v) {
    bits_.push_back(bit);
  }
  return *this;
}

size_t BitStream::size() const
{
  return bits_.size();
}

void BitStream::pad()
{
  while (bits_.size() % CHAR_SIZE != 0) {
    bits_.push_back(false);
  }
}

bool BitStream::operator[](size_t i) const
{
  return bits_.at(i);
}

const std::vector<bool>& BitStream::dump() const
{
  return bits_;
}

BitStream& BitStream::operator<<(int i)
{
  append(i);
  return *this;
}

void BitStream::append(int i)
{
  append(i, static_cast<int>(ceil(log2(i))));
}

void BitStream::append(int i, int num_bits)
{
  for (auto j = num_bits; j > 0; --j) {
    bits_.push_back((bool) (i >> (j - 1) & 1));
  }
}

bool operator==(const BitStream& lhs, const BitStream& rhs)
{
  if (lhs.size() != rhs.size()) {
    return false;
  }
  for (size_t i = 0; i < lhs.size(); ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
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

