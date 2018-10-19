/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_DELTA_CODER_H
#define CAPS_DELTA_CODER_H

// Include C standard libraries.

// Include C++ standard libraries.

// Include other headers from this project.
#include "coder.h"
#include "gamma_coder.h"
#include "binary_coder.h"

// Include headers from other projects.

template <typename IntType, typename BitVectorType>
class DeltaCoder: public Coder<IntType, BitVectorType>
{
 public:

  DeltaCoder()
    : gamma_coder_{}, binary_coder_{}
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
    return gamma_coder_.value_size(binary_size);
  }

 protected:

  void encode_impl(const IntType& value, BitVectorType* encoding) const override
  {
    std::unique_ptr<BitVectorType> buffer{binary_coder_.encode(value)};
    gamma_coder_.encode(buffer->size(), encoding);
    for (size_t i = 1; i < buffer->size(); ++i) {
      encoding->push_back((*buffer)[i]);
    }
  }

  std::optional<std::pair<IntType, size_t>> decode_impl(
    const BitVectorType& buffer, const size_t position) const override
  {
    auto decoded_length = gamma_coder_.decode(buffer, position);
    if (!decoded_length.has_value()) {
      return std::nullopt;
    }
    const auto& [length, length_size] = *decoded_length;

    binary_coder_.set_width(length_size - 1);
    auto decoded_value = binary_coder_.decode(buffer, position + length_size);
    binary_coder_.set_width(0);
    if (!decoded_value.has_value()) {
      return std::nullopt;
    }
    const auto& [value, value_size] = *decoded_value;

    return std::make_optional(std::make_pair(
      value + (1 << length), length_size + value_size - 1));
  }

 private:

  GammaCoder<size_t, BitVectorType> gamma_coder_;
  mutable BinaryCoder<IntType, BitVectorType> binary_coder_;

};

#endif //CAPS_DELTA_CODER_H
