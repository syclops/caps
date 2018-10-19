/**
 * Interface description for the abstract base class of a binary
 * encoder/decoder.
 *
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_CODER_H
#define CAPS_CODER_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <optional>
#include <utility>

// Include other headers from this project.

// Include headers from other projects.


/**
 * Abstract base class for encoding and decoding data types in binary.
 *
 * The base class provides two inverse methods: encode and decode.
 *
 * @tparam DataType - the type of data to encode/decode.
 * @tparam EncodingType - the type of the encoded data (a bit vector or array).
 */
template <typename DataType, typename EncodingType>
class Coder
{
 public:

  // Default constructor is fine.
  Coder() = default;

  // Default rule of five member functions are fine.

  Coder(const Coder&) = default;

  Coder(Coder&&) noexcept = default;

  Coder& operator=(const Coder&) = default;

  Coder& operator=(Coder&&) noexcept = default;

  virtual ~Coder() = default;

  /**
   * Encode a data value and append it to a bitvector (possibly a newly
   * created bitvector). If the value is invalid (e.g., passing a negative
   * integer to a coder that expects a nonnegative integer), return a null
   * pointer.
   *
   * Callers of this function are responsible for ensuring that the lifetime
   * of the objects pointed to by encoding and by the return value (if
   * different from encoding) are managed correctly.
   *
   * @param value - the data value to encode.
   * @param encoding - the encoding value to pass
   * @return - if there is some error during encoding, such as an invalid
   *           value, return the null pointer. If encoding is the null
   *           pointer, create and return a pointer to a new EncodingType
   *           instance containing the binary encoding of value. Otherwise,
   *           append the bits to the instance pointed to by encoding and
   *           return encoding.
   */
  EncodingType* encode(const DataType& value, EncodingType* encoding = nullptr,
                       bool reserve_size = false) const
  {
    // If the value is invalid, return the null pointer.
    if (!valid_value(value)) {
      return nullptr;
    }

    // Set the output buffer and reserve space if necessary.
    auto buffer = encoding;
    if (buffer == nullptr) {
      buffer = new EncodingType();
    }
    if (reserve_size) {
      buffer->reserve(buffer->size() + value_size(value));
    }

    // Call the internal implementation of the encoding algorithm.
    encode_impl(value, buffer);
    return buffer;
  }

  /**
   * Decode part of a binary bit vector into a DataType.
   *
   * @param buffer - a reference to the buffer to decode.
   * @param position - the position in the buffer to begin decoding from.
   * @return - if there is an error with decoding, return std::nullopt as an
   *           error value. Otherwise, return the decoded DataType instance
   *           along with the number of bits decoded.
   */
  std::optional<std::pair<DataType, size_t>> decode(const EncodingType& buffer,
    const size_t position = 0) const
  {
    if (buffer.size() <= position) {
      return std::nullopt;
    }
    // To avoid declaring default parameters in a virtual function, this
    // function cannot be overridden and simply calls decode_impl internally.
    return decode_impl(buffer, position);
  }

  /**
   * Whether or not a value can be encoded by the coder or not.
   *
   * @param value - a constant reference to the value to be encoded.
   * @return - true if the coder can encode the value and false otherwise.
   */
  virtual inline bool valid_value(const DataType& value) const = 0;

  /**
   * Calculate the size of a value's encoding.
   *
   * @param value - a constant reference to the value to be encoded.
   * @return - the size of the encoding of the value in bits.
   */
  virtual inline size_t value_size(const DataType& value) const = 0;

 protected:

  /**
   * Implement the encoding algorithm for the coder.
   *
   * The high-level specifications for this member function are provided in
   * the specification for encode, and the detailed specifications with
   * respect to the encoding are given in overriding member functions.
   *
   * @param value - the value to encode.
   * @param encoding - a pointer to the EncodingType object in which to place
   *                   the encoding (nullptr to create a new object).
   * @return - nullptr in case of error, or an EncodingType object containing
   *           the new encoding at the end.
   */
  virtual void encode_impl(const DataType& value,
                           EncodingType* encoding) const = 0;

  /**
   * Implement the decoding algorithm for the coder.
   *
   * The high-level specifications for this member function are provided in
   * the specification for decode, and the detailed specifications with
   * respect to the decoding are given in overriding member functions.
   *
   * @param buffer - the buffer from which to read.
   * @param position - the starting index in the buffer to read from.
   * @return - a pair representing the decoded data and the number of bits
   *           read from the buffer. If there is an error in decoding, return
   *           std::nullopt.
   */
  virtual std::optional<std::pair<DataType, size_t>> decode_impl(
    const EncodingType& buffer, size_t position) const = 0;
};

#endif //CAPS_CODER_H
