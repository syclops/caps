
// Include the header file.
#include "huffmandebug.h"

// Include C standard libraries.
#include <cstdlib>

// Include C++ standard libraries.
#include <map>
#include <string>
#include <unordered_map>

#include "../../src/cpp/signal_authority/huffman.h"

HuffmanDebug::HuffmanDebug(const std::map<std::string, size_t>& counts)
  : HuffmanTree(counts)
{
  // Nothing to do here.
}

HuffmanDebug::HuffmanDebug(
  const std::unordered_map<std::string, size_t>& counts)
  : HuffmanTree(counts)
{
  // Nothing to do here.
}

size_t HuffmanDebug::hash_size() const
{
  size_t size = 0;
  for (auto it: codebook_.left) {
    size += 32 + it.second.size();
  }
  return size;
}

