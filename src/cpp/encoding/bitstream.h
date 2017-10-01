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

  size_t size() const;

  static const size_t CHAR_SIZE = 8;

  void pad();

 private:

  std::vector<bool> bits_;
};

std::ostream& operator<<(std::ostream& out_stream, BitStream& bitstream);

#endif //CAPS_BITSTREAM_H
