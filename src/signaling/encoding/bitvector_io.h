/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_BITVECTOR_IO_H
#define CAPS_BITVECTOR_IO_H

// Include C standard libraries.
#include <cstddef>

// Include C++ standard libraries.
#include <iostream>

// Include other headers from this project.
#include "bitvector/bitvector.h"
#include "coder/char_coder.h"

// Include headers from other projects.

template <typename BitContainerType>
std::ostream& operator<<(std::ostream& os,
                         const BitVector<BitContainerType>& bv)
{
//  bv.pad();
  CharCoder<BitVector<BitContainerType>> char_coder;
  size_t position = 0;
  while (position < bv.size()) {
    auto decoded = char_coder.decode(bv, position);
    if (!decoded.has_value()) {
      break;
    }
    os << decoded->first;
    position += decoded->second;
  }
  return os;
}

// TODO: template specialization for operator<< with boost:dynamic_bitset

// TODO: operator>>
//template <typename BitContainerType>
//std::istream& operator>>(std::istream& is,
//                         const BitVector<BitContainerType>& bv);

#endif //CAPS_BITVECTOR_IO_H
