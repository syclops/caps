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

// Include other headers from this project.

// Include header from other projects.

BitStream& BitStream::operator<<(bool b)
{
  bits_.push_back(b);
  return *this;
}

BitStream& BitStream::operator<<(char c)
{
  for (size_t i = CHAR_SIZE; i > 0; --i) {
    bits_.push_back((bool) (c >> (i - 1) & 1));
  }
  return *this;
}

BitStream& BitStream::operator<<(const std::string& s)
{
  for (char c: s) {
    *this << c;
  }
  return *this;
}

size_t BitStream::size() const
{
  return bits_.size();
}

void BitStream::pad()
{
  while (bits_.size() % CHAR_SIZE) {
    bits_.push_back(0);
  }
}

std::ostream& operator<<(std::ostream& out_stream, BitStream& bitStream)
{
  bitStream.pad();
  return out_stream;
}