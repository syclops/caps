/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_CHAR_CODER_H
#define CAPS_CHAR_CODER_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <limits>

// Include other headers from this project.
#include "coder.h"
#include "../../common/defs.h"
#include "../../common/bitvector/bitvector.h"

// Include headers from other projects.

template <typename EncodingType = BitVector<>>
class CharCoder: public Coder<char, EncodingType>
{

 public:

  inline bool valid_value(const char&) const override
  {
    return true;
  }

  inline size_t value_size(const char&) const override
  {
    return BITS_IN_CHAR;
  }

 protected:

  void encode_impl(const char& value, EncodingType* buffer) const override
  {
    for (size_t i = BITS_IN_CHAR; i > 0; --i) {
      buffer->push_back(static_cast<bool>(value >> (i - 1) & 1));
    }
  }

  std::optional<std::pair<char, size_t>> decode_impl(
    const EncodingType& buffer, const size_t position) const override
  {
    if (position + BITS_IN_CHAR > buffer.size()) {
      return std::nullopt;
    }
    auto c = static_cast<char>(0);
    for(size_t i = position; i < position + BITS_IN_CHAR; ++i) {
      c = (c << 1) + buffer[i];
    }
    return std::make_optional(std::make_pair(c, BITS_IN_CHAR));
  }
};

#endif //CAPS_CHAR_CODER_H
