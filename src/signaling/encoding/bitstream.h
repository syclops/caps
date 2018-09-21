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

class BitStream
{
 public:
  // Default constructors, assignment operators, and destructor.

  BitStream& operator<<(bool b);
  BitStream& operator<<(char c);
  BitStream& operator<<(const std::string& s);
  BitStream& operator<<(const std::vector<bool>& v);
  BitStream& operator<<(int i);

  void append(int i);
  void append(int i, int num_bits);

  const std::vector<bool>& dump() const;

  bool operator[](size_t i) const;

  size_t size() const;

  static const size_t CHAR_SIZE = 8;

  void pad();

 private:

  std::vector<bool> bits_;
};

bool operator==(const BitStream& lhs, const BitStream& rhs);

std::ostream& operator<<(std::ostream& out_stream, BitStream& bitstream);

#endif //CAPS_BITSTREAM_H
