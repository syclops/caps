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

template <typename IntType, typename BitVectorType>
class IntegerCoder
{
 public:

  IntegerCoder() = default;

  virtual ~IntegerCoder() = default;

  virtual BitVectorType encode(IntType value) = 0;

  /**
   * Decode an integer from a buffer of bits.
   *
   * @param buffer - the buffer of bits from which to decode.
   * @param position - the index in the buffer from which to begin reading bits.
   * @return - a decoded integer and the number of bits consumed (0 if
   *           decoding failed).
   */
  virtual std::pair<IntType, size_t> decode(BitVectorType buffer,
                                            size_t position = 0) = 0;
};

template <typename IntType, typename BitVectorType>
class UnaryCoder: public IntegerCoder<IntType, BitVectorType>
{
 public:

  UnaryCoder(bool terminator = true)
    : terminator_{terminator}
  {
    // Nothing to do here.
  }

  BitVectorType encode(IntType value)
  {
    BitVectorType buffer;
    buffer.resize(value, !terminator_);
    buffer.push_back(terminator_);
    return buffer;
  }

  std::pair<IntType, size_t> decode(BitVectorType buffer, size_t position)
  {
    if (buffer.size() <= position) {
      return {0, 0};
    }
    for (size_t i = position; i < buffer.size(); ++i) {
      if (buffer[i] == terminator_) {
        return {i - 1, 1};
      }
    }
    return {0, 0};
  }

 private:

  bool terminator_;
};

template <typename IntType, typename BitVectorType>
class BinaryCoder: public IntegerCoder<IntType, BitVectorType>
{
 public:

  BinaryCoder(size_t width = 0)
    : width_{width}
  {
    // Nothing to do here.
  }

  BitVectorType encode(IntType value)
  {
    BitVectorType buffer;
    auto width = width_;
    if (width == 0) {
      width = bit_width(value);
    }
    if (value > (1 << (width + 1)) - 1) {
      return buffer;
    }
    buffer.reserve(width);
    for (auto i = width; i > 0; --i) {
      buffer.push_back((1 << (i - 1) & value) != 0);
    }
    return buffer;
  }

  std::pair<IntType, size_t> decode(BitVectorType buffer, size_t position)
  {
    if (buffer.size() <= position) {
      return {0, 0};
    }
    auto width = buffer.size() - position;
    if (width_ != 0 && width_ < width) {
      width = width_;
    }
    IntType value = 0;
    for (size_t i = position; i < width; ++i) {
      value <<= 1;
      value += (buffer[i] ? 1 : 0);
    }
    return {value, width};
  }

 private:

  size_t width_;
};

template <typename IntType, typename BitVectorType>
class GammaCoder: public IntegerCoder<IntType, BitVectorType>
{
 public:

  BitVectorType encode(IntType value)
  {
    BitVectorType buffer;
    auto width = bit_width(value);
    buffer.resize(width, false);
    auto coder = BinaryCoder<IntType, BitVectorType>(width);
    auto binary = coder.encode(value);
    buffer.push_back(binary);
    return buffer;
  }

  std::pair<IntType, size_t> decode(BitVectorType buffer, size_t position)
  {
    if (buffer.size() <= position) {
      return {0, 0};
    }
    auto unary_coder = UnaryCoder<size_t, BitVectorType>(true);
    auto width_pair = unary_coder.decode(buffer, position);
    if (width_pair.second == 0
        || buffer.size() - position < 2 * width_pair.first) {
      return {0, 0};
    }
    auto binary_coder = BinaryCoder<IntType, BitVectorType>(width_pair.first);
    return binary_coder.decode(buffer, position + width_pair.second - 1);
  }

};

template <typename IntType, typename BitVectorType>
class DeltaCoder: public IntegerCoder<IntType, BitVectorType>
{
 public:

  BitVectorType encode(IntType value)
  {
    GammaCoder<size_t, BitVectorType> gamma_coder;
    auto width = bit_width(value);
    BitVectorType buffer(gamma_coder.encode(width));
    BinaryCoder<IntType, BitVectorType> binary_coder(width);
    auto binary_encoding = binary_coder.encode(value);
    for (size_t i = 1; i < binary_encoding.size(); ++i) {
      buffer.push_back(binary_encoding[i]);
    }
    return buffer;
  }

  std::pair<IntType, size_t> decode(BitVectorType buffer, size_t position)
  {
    // Gamma decode the buffer to get the length of the remainder. If this
    // step fails or returns a length of 0, return an error.
    GammaCoder<size_t, BitVectorType> gamma_coder;
    auto width_pair = gamma_coder.decode(buffer, position);
    if (width_pair.first == 0 || width_pair.second == 0) {
      return {0, 0};
    }

    // If the decoded length is 1, then the number is 1 and there are no
    // additional bits in the encoding beyond what was already consumed.
    if (width_pair.first == 1) {
      return width_pair;
    }

    // Binary decode the remainder of the bits, and add the leading 1 bit to
    // the final value (as it is implicitly encoded).
    BinaryCoder<IntType, BitVectorType> binary_coder(width_pair.first - 1);
    auto value_pair = binary_coder.decode(buffer, position + width_pair.second);
    if (value_pair.second == 0) {
      return {0, 0};
    }
    return {value_pair.first + (1 << width_pair.first),
            width_pair.second + value_pair.second};
  }
};

#endif //CAPS_INTEGER_H
