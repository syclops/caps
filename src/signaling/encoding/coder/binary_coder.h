/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_BINARY_CODER_H
#define CAPS_BINARY_CODER_H

// Include C standard libraries.
#include <cmath>
#include <cstdlib>

// Include C++ standard libraries.

// Include other headers from this project.
#include "coder.h"
#include "../bitvector/bitvector.h"

// Include headers from other projects.

template <typename IntType, typename EncodingType = BitVector<>>
class BinaryCoder: public Coder<IntType, EncodingType>
{
 public:

  explicit BinaryCoder(size_t width = 0)
    : width_{width}
  {
    // Nothing to do here.
  }

  inline bool valid_value(const IntType& value) const override
  {
    // The coder cannot encode negative numbers.
    if (value < 0) {
      return false;
    }

    // If the codeword width is zero, the coder can encode any nonnegative
    // value. Otherwise, make sure the value fits into the codeword width.
    return width_ == 0 || value < (1 << width_);
  }

  inline size_t value_size(const IntType& value) const override
  {
    if (width_ != 0) {
      return width_;
    } else if (value == 0) {
      return 1;
    } else {
      return static_cast<size_t>(floor(log2(value)) + 1);
    }
  }

 protected:

  void encode_impl(const IntType& value, EncodingType* encoding) const override
  {
    auto width = width_;
    if (width == 0) {
      width = value_size(value);
    }
    for (auto i = width; i > 0; --i) {
      encoding->push_back((1 << (i - 1) & value) != 0);
    }
    return encoding;
  }

  std::optional<std::pair<IntType, size_t>> decode_impl(
    const EncodingType& buffer, size_t position) const override
  {
    if (width_ != 0 && buffer.size() <= position + width_) {
      return std::nullopt;
    }
    IntType value = 0;
    auto end_position = width_ != 0 ? position + width_ : buffer.size();
    for (auto i = position; i < end_position; ++i) {
      value = (value << 1) + static_cast<IntType>(buffer[i]);
    }
    return std::make_optional(std::make_pair(value, end_position - position));
  }


 private:

  size_t width_;
};

#endif //CAPS_BINARY_CODER_H
