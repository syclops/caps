/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_STRING_CODER_H
#define CAPS_STRING_CODER_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <optional>
#include <utility>

// Include other headers from this project.
#include "char_coder.h"
#include "coder.h"
#include "../defs.h"
#include "../bitvector/bitvector.h"

// Include headers from other projects.
template <typename StringType = std::string,
          typename EncodingType = BitVector<>>
class StringCoder: public Coder<StringType, EncodingType>
{
 public:

  // Compiler generated methods are fine.

  inline bool valid_value(const StringType& value) const override
  {
    return !value.empty();
  }

  inline size_t value_size(const StringType& value) const override
  {
    return BITS_IN_CHAR * value.size();

  }

 protected:

  /**
   * Implement the encoding algorithm for the coder.
   *
   * Specifically, for a string s, add the characters of the string to
   * the end of the returned encoding, where the last character has its
   *
   * @param value - as defined in the parent method.
   * @param encoding - as defined in the parent method.
   * @return - as defined in the parent method.
   */
  void encode_impl(const StringType& value,
                            EncodingType* buffer) const override
  {
    for (auto itr = value.begin(); itr != value.end(); ++itr) {
      // We use encode instead of encode_impl because the latter is protected
      // in CharCoder. Discard the return value for each of these calls.
      if (std::next(itr) == value.end()) {
        char_coder_.encode(static_cast<char>(*itr | LAST_CHAR_BIT), buffer);
      } else {
        char_coder_.encode(*itr, buffer);
      }
    }
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
  std::optional<std::pair<StringType, size_t>> decode_impl(
    const EncodingType& buffer, const size_t position) const override
  {
    std::string value;

    for (size_t i = position; i < buffer.size(); i += BITS_IN_CHAR) {
      // Attempt to decode a character, and return an error if this step fails.
      auto decoded = char_coder_.decode(buffer, i);
      if (!decoded.has_value()) {
        return std::nullopt;
      }

      // Otherwise, check whether the character is the last in the string. If
      // it is, return the completed string. Otherwise, just append the
      // character.
      auto decoded_char = decoded.first.value();
      if(decoded_char & LAST_CHAR_BIT) {
        value.push_back(decoded_char - LAST_CHAR_BIT);
        return std::make_optional(std::make_pair(value, value_size(value)));
      } else {
        value.push_back(decoded_char);
      }
    }

    // If we reach the end of the buffer and haven't found the last
    // character, return an error.
    return std::nullopt;
  }

 private:

  constexpr static char LAST_CHAR_BIT = static_cast<char>(0x80);
  const CharCoder<EncodingType> char_coder_;

};

#endif //CAPS_STRING_CODER_H
