/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_GAMMA_CODER_H
#define CAPS_GAMMA_CODER_H

// Include C standard libraries.
#include <cstddef>

// Include C++ standard libraries.
#include <optional>
#include <utility>

// Include other headers from this project.
#include "coder.h"
#include "unary_coder.h"
#include "binary_coder.h"

// Include headers from other projects.

template <typename IntType, typename BitVectorType>
class GammaCoder: public Coder<IntType, BitVectorType>
{
 public:

  GammaCoder()
    : unary_coder_{}, binary_coder_{}
  {
    // Nothing to do here.
  }

  inline bool valid_value(const IntType& value) const override
  {
    return value >= 0;
  }

  inline size_t value_size(const IntType& value) const override
  {
    auto binary_size = binary_coder_.value_size(value);
    return unary_coder_.value_size(binary_size) + binary_size - 1;
  }

 protected:

  void encode_impl(const IntType& value, BitVectorType* encoding) const override
  {
    std::unique_ptr<BitVectorType> buffer{binary_coder_.encode(value)};
    unary_coder_.encode(buffer->size(), encoding);
    encoding->pop_back();
    encoding->push_back(*buffer);
  }

  std::optional<std::pair<IntType, size_t>> decode_impl(
    const BitVectorType& buffer, const size_t position) const override
  {
    auto decoded_length = unary_coder_.decode(buffer, position);
    if (!decoded_length.has_value()) {
      return std::nullopt;
    }
    const auto& [length, length_size] = *decoded_length;

    binary_coder_.set_width(length_size);
    auto decoded_value = binary_coder_.decode(
      buffer, position + length_size - 1);
    binary_coder_.set_width(0);
    if (!decoded_value.has_value()) {
      return std::nullopt;
    }
    const auto& [value, value_size] = *decoded_value;

    return std::make_optional(std::make_pair(value,
                                             length_size + value_size - 1));
  }

 private:

  UnaryCoder<size_t, BitVectorType> unary_coder_;
  mutable BinaryCoder<IntType, BitVectorType> binary_coder_;
};

#endif //CAPS_GAMMA_CODER_H
