/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_UNARY_CODER_H
#define CAPS_UNARY_CODER_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <optional>
#include <utility>

// Include other headers from this project.
#include "coder.h"
#include "../../common/bitvector/bitvector.h"

// Include headers from other projects.

template <typename IntType, typename EncodingType = BitVector<>>
class UnaryCoder: public Coder<IntType, EncodingType>
{
 public:

  UnaryCoder(bool terminator = true)
    : terminator_{terminator}
  {
    // Nothing to do here.
  }

  inline bool valid_value(const IntType& value) const override
  {
    return value >= 0;
  }

  inline size_t value_size(const IntType& value) const override
  {
    return value + 1;
  }

 protected:

  /**
   * Implement the encoding algorithm for the coder.
   *
   * Specifically, for a nonnegative integer n, add the unary code for n to
   * the end of the returned encoding, which (assuming that terminator_ is
   * true) consists of n falses followed by a true.
   *
   * @param value - as defined in the parent method.
   * @param encoding - as defined in the parent method.
   * @return - as defined in the parent method.
   */
  void encode_impl(const IntType& value, EncodingType* buffer) const override
  {
    // Append the unary encoding to the buffer.
    for (IntType i = 0; i < value; ++i) {
      buffer->push_back(!terminator_);
    }
    buffer->push_back(terminator_);
  }

  /**
   * Implement the decoding algorithm for the coder.
   *
   * Specifically, starting at index position in the buffer, read the number
   * of zeros (assuming that terminator_ is true) until reaching a true. Then
   * return this number as the decoded value, along with this number plus one
   * as the number of decoded bits. If the buffer does not contain a
   * terminator after the starting position, return std::nullopt.
   * bits returned.
   *
   * @param buffer - as defined in the parent method.
   * @param position - as defined in the parent method.
   * @return - as defined in the parent method.
   */
  std::optional<std::pair<IntType, size_t>> decode_impl(
    const EncodingType& buffer, const size_t position) const override
  {
    for (size_t i = position; i < buffer.size(); ++i) {
      if (buffer[i] == terminator_) {
        return std::make_optional(std::make_pair(i - position,
                                                 i - position + 1));
      }
    }
    return std::nullopt;
  }

 private:

  bool terminator_;
};

#endif //CAPS_UNARY_CODER_H
