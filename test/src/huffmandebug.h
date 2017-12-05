//
// Created by smaptas on 30.08.17.
//

#ifndef CAPS_HUFFMAN_TESTER_H
#define CAPS_HUFFMAN_TESTER_H

#include "../../src/cpp/encoding/huffman.h"

class HuffmanDebug: public HuffmanCoder
{
 public:
  /**
   * Construct a Huffman codebook from an ordered map.
   * @param counts
   */
  HuffmanDebug(const std::map<std::string, size_t>& counts);

  /**
   * Construct a Huffman codebook from an unordered map.
   * @param counts
   */
  HuffmanDebug(const std::unordered_map<std::string, size_t>& counts);

  size_t hash_size() const;
};

#endif //CAPS_HUFFMAN_TESTER_H
