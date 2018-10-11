/**
 * Author: smaptas
 */

#ifndef CAPS_BITSTREAM_H
#define CAPS_BITSTREAM_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <iostream>
#include <string>
#include <vector>

// Include other headers from this project.

// Include headers from other projects.

/**
 * Number of bits in a byte. This shouldn't ever change, but you never know.
 */
const size_t BITS_IN_BYTE = 8;

/**
 * Bitvector that handles the addition of other common data types.
 */
class BitStream: public std::vector<bool>
{
 public:
  // Inherit constructor, destructor, and assignment operator.
  // TODO: Add constructors from char, int, string, vector<bool>

  // Inherit iterators. Note that these do not conform exactly to standard
  // vector iterator behavior.

  //////////////////////////////////////////////////////////////////////////////
  // Accessors
  //////////////////////////////////////////////////////////////////////////////

  // TODO: possibly add ways to access characters, ints, strings, etc.
  // starting at a certain position.

  //////////////////////////////////////////////////////////////////////////////
  // Modifiers
  //////////////////////////////////////////////////////////////////////////////

  // TODO: overload assign(), insert(), emplace(), and emplace_back() to handle
  // other data types such as chars, strings, ints, etc.

  /**
   * Append the binary encoding of a character to the Bitstream.
   *
   * @param c - the character to encode and append.
   */
  void push_back(char c);

  /**
   * Append the binary encoding of a string to the Bitstream.
   *
   * The encoding is done by appending the encoding of each character to the
   * Bitstream in sequence. The ordering of the characters is big-endian,
   * that is, the encoding of the characters that occur first in the string
   * will be at a lower index than of characters that occur later.
   *
   * @param s - a reference to the string to encode and append.
   */
  void push_back(const std::string& s);

  /**
   * Append the binary encoding of a string rvalue to the Bitstream.
   *
   * The encoding is performed in the same way as for a string passed by
   * constant reference, except that move semantics are used.
   *
   * @param s - an rvalue reference to the string to encode and append.
   */
  void push_back(std::string&& s);

  /**
   * Append a vector of boolean values to the Bitstream.
   *
   * @param v - a constant reference to the vector to append.
   */
  void push_back(const std::vector<bool>& v);

  /**
   * Append a vector of boolean values (as an rvalue reference) to the
   * Bitstream.
   *
   * @param v - an rvalue reference to the vector to append.
   */
  void push_back(std::vector<bool>&& v);

  /**
   * Append the binary encoding of a nonnegative integer to the end of the
   * Bitstream.
   *
   * @tparam IntType - an integer type (short, int, long, long long, or an
   *                   unsigned version of one of those).
   * @param i - the integer to encode in binary.
   */
  template<typename IntType>
  void push_back(IntType i);

  /**
   * Pad a Bitstream to a multiple of `CHAR_SIZE` bits.
   *
   * This allows the Bitstream to be written as a sequence of characters to an
   * output stream.
   *
   * @param b - the boolean value to pad the Bitstream with (false by default
   */
  void pad(bool b = false);

  /**
   * Size of a char in bits.
   */
  static constexpr size_t CHAR_SIZE = BITS_IN_BYTE * sizeof(char);
};

/**
 * Check whether two Bitstream instances are equal.
 *
 * Equality is defined as having the same ordered bitwise contents.
 *
 * @param lhs - the left-hand side Bitstream to compare.
 * @param rhs - the right-hand side Bitstream to compare.
 * @return - true if the Bitstreams are equal and false otherwise.
 */
bool operator==(const BitStream& lhs, const BitStream& rhs);

/**
 * Check whether two Bitstream instances are not equal.
 *
 * @param lhs - the left-hand side Bitstream to compare.
 * @param rhs - the right-hand side Bitstream to compare.
 * @return - false if the Bitstreams are equal and true otherwise.
 */
bool operator!=(const BitStream& lhs, const BitStream& rhs);

/**
 * Write the contents of a Bitstream to an ostream instance.
 *
 * The Bitstream is padded to the nearest character size (in bits) before
 * writing, so that it can be written as a sequence of chars.
 *
 * @param out_stream - the ostream instance to write to.
 * @param bitstream - a reference to the bitstream to write.
 * @return - a reference to the ostream that was written to.
 */
std::ostream& operator<<(std::ostream& out_stream, BitStream& bitstream);

#endif //CAPS_BITSTREAM_H
