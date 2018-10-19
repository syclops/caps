/**
 * Integer Encoding/Decoding Library
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_INTEGER_H
#define CAPS_INTEGER_H

// Include C standard libraries.
#include <cmath>
#include <cstdlib>

// Include C++ standard libraries.
#include <utility>

// Include other headers from this project.

// Include headers from other projects.

template <typename IntType, typename BitVectorType>
inline BitVectorType encode_int(IntType value, size_t num_bits = 0)
{
  if (num_bits == 0) {
    num_bits = static_cast<size_t>(ceil(log2(value)));
  }
  BitVectorType encoding;
  for (auto i = num_bits; i > 0; --i) {
    encoding.push_back((1 << (i - 1) & value) != 0);
  }
  return encoding;
}

template <typename IntType, typename BitVectorType>
inline IntType decode_int(const BitVectorType& bitvector)
{
  IntType value = 0;
  for (size_t i = 0; i < bitvector.size(); ++i) {
    value <<= 1;
    value += (bitvector[i] ? 1 : 0);
  }
  return value;
}

template <typename IntType>
size_t bit_width(IntType value)
{
  if (value == 0) {
    return 1;
  }
  return static_cast<size_t>(floor(log2(value)) + 1);
}

#endif //CAPS_INTEGER_H
