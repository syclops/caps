/**
 * Huffman encoding and decoding functions.
 *
 * Author: Steve Matsumoto
 *
 * TODO: License information
 */

#ifndef CAPS_HUFFMAN_H_
#define CAPS_HUFFMAN_H_

// Include C standard library headers.
#include <cstdlib>

// Include C++ standard library headers.
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

// Include other headers from this project.
#include "integer.h"

// Include headers from other projects.

/**
 * Return type trait for a sequence of symbols.
 *
 * This struct contains a type alias declaration to determine the return type of
 * Huffman decoding a stream of bits. For most types, this is simply a vector of
 * symbols, so the template defines this as the return type by default.
 *
 * @tparam SymbolType - the type of symbol that the HuffmanCoder decodes a
 *                      single codeword (represented as a vector of bits) as.
 */
template <typename SymbolType>
struct symbol_sequence
{
  using type = std::vector<SymbolType>;

  symbol_sequence()
    : sequence{}
  {
    // Nothing to do here.
  }

  explicit symbol_sequence(type sequence)
    : sequence{sequence}
  {
    // Nothing to do here.
  }

  /**
   * Append a single symbol to the end of the sequence.
   * @param symbol - the symbol to append.
   */
  void append(SymbolType symbol)
  {
    sequence.push_back(symbol);
  }

  type sequence;
};

/**
 * Sequence of string symbols decoded from a Huffman bitstream.
 *
 * This struct defines a sequence of chars to be a single string rather than a
 * vector of chars. Thus appending a new char to the end of a sequence of chars
 * adds the char to the end of the existing string.
 */
template <>
struct symbol_sequence<char>
{
  using type = std::string;

  symbol_sequence()
    : sequence{}
  {
    // Nothing to do here.
  }

  explicit symbol_sequence(type sequence)
    : sequence{sequence}
  {
    // Nothing to do here.
  }

  void append(char symbol)
  {
    sequence += symbol;
  }

  type sequence;
};

/**
 * Huffman encoder and decoder for arbitrary types of keys.
 *
 * This class creates a Huffman encoding and decoding codebook given a list
 * of symbols and their associated counts. The codebook is generated at
 * construction time.
 *
 * @tparam Key - the type of symbols to encode using Huffman coding.
 * @tparam BitVectorType - the type of the output encoding.
 */
template <typename Key, typename BitVectorType>
class HuffmanCoder
{
 public:

  /**
   * Constructor.
   *
   * The type of counts must support iteration through the container in
   * key-value pairs.
   *
   * @tparam MapType - a mapping-like type.
   * @param counts - a mapping of symbols to counts.
   */
  template <typename MapType>
  explicit HuffmanCoder(const MapType& counts)
    : encoding_map_{}, decoding_symbols_{}, decoding_indices_{},
      decoding_codes_{}, max_length_{0}
  {
    create_codebook_generic(counts);
  }

  /**
   * Get the map corresponding to the encoding codebook.
   *
   * @return - a map of each symbol to its encoding.
   */
  std::unordered_map<Key, BitVectorType> encoding_map() const
  {
    return encoding_map_;
  }

  /**
   * Get the maps corresponding to the decoding codebook.
   *
   * The codebook is returned in compact form: a triple of arrays that take
   * up minimal space but allow for decoding of bits into symbols.
   *
   * @return - a triple of vectors representing the codebook.
   */
  std::tuple<std::vector<Key>, std::vector<size_t>, std::vector<int>>
  decoding_map() const
  {
    return std::make_tuple(decoding_symbols_, decoding_indices_,
                           decoding_codes_);
  };

  /**
   * Encode a sequence of symbols.
   *
   * @param sequence - the sequence of symbols to encode.
   * @return - a bitvector representing the encoding of the symbols.
   */
  BitVectorType encode(typename symbol_sequence<Key>::type sequence) const
  {
    BitVectorType encoded;
    symbol_sequence<Key> symbols{sequence};
    for (auto symbol: symbols.sequence) {
      encoded.push_back(encoding_map_.at(symbol));
    }
    return encoded;
  }

  /**
   * Decode a codeword's bits into a sequence of symbols.
   *
   * @param codeword - a bitvector containing the bits to decode.
   * @return - a sequence of symbols whose encoding is the codeword bits.
   */
  typename symbol_sequence<Key>::type decode(
    const BitVectorType& codeword) const
  {
    size_t index = 0;
    symbol_sequence<Key> decoded;
    while (codeword.size() - index > 0) {

      // Create a buffer of bits whose length is the maximum encoding size,
      // and fill it with bits from the remainder of the codeword (pad with 0s).
      BitVectorType current_bits(max_length_);
      for (size_t i = 0; i < max_length_; ++i) {
        if (index + i >= codeword.size()) {
          current_bits[i] = false;
        } else {
          current_bits[i] = codeword[index + i];
        }
      }

      // Decode the buffer, obtaining a symbol and the length of bits
      // consumed, add the decoded symbol to the output buffer, and advance the
      // index in the codeword by the number of bits consumed.
      auto decoded_symbol = decode_symbol(current_bits);
      decoded.append(decoded_symbol.first);
      index += decoded_symbol.second;
    }
    return decoded.sequence;
  }

 private:

  /**
   * A node in the Huffman tree.
   *
   * Note: each leaf node *should* be owned by only one non-leaf node since
   *       Huffman trees are binary; however, because we use a custom
   *       comparator function when constructing the tree, we cannot use a
   *       unique_ptr to indicate the child nodes.
   */
  struct HuffmanNode
  {

    /**
     * Create a leaf node in the Huffman tree.
     *
     * @param symbol - the symbol represented by the node.
     * @param count - the count of the symbol in the input corpus.
     */
    HuffmanNode(Key symbol, int count)
      : symbol{std::shared_ptr<Key>(new Key(symbol))}, count{count}, size{1},
        zero{}, one{}
    {
      // Nothing to do here.
    }

    /**
     * Create a non-left node in the Huffman tree.
     *
     * @param zero - a pointer to the Huffman node following the zero edge.
     * @param one - a pointer to the Huffman node following the one edge.
     */
    HuffmanNode(std::shared_ptr<HuffmanNode> zero,
                std::shared_ptr<HuffmanNode> one)
      : symbol{std::shared_ptr<Key>()}, count{zero->count + one->count},
        size{zero->size + one->size}, zero{std::move(zero)}, one{std::move(one)}
    {
      // Nothing to do here.
    }

    std::shared_ptr<Key> symbol;
    int count;
    int size;
    std::shared_ptr<HuffmanNode> zero;
    std::shared_ptr<HuffmanNode> one;
  };

  /**
   * Comparison functor for Huffman node pointers.
   */
  struct HuffmanNodeComp
  {
    bool operator()(const std::shared_ptr<HuffmanNode> lhs,
                    const std::shared_ptr<HuffmanNode> rhs) const
    {
      if (lhs->count == rhs->count) {
        if (lhs->size == rhs->size) {
          if (lhs->size == 1) {
            return lhs->symbol > rhs->symbol;
          }
          return true;
        }
        return lhs->size > rhs->size;
      }
      return lhs->count > rhs->count;
    }
  };

  /**
   * Comparison functor for a key-count pair.
   */
  struct CodebookComp
  {
    bool operator()(const std::pair<Key, size_t> lhs,
                    const std::pair<Key, size_t> rhs) const
    {
      if (lhs.second == rhs.second) {
        return lhs.first < rhs.first;
      }
      return lhs.second < rhs.second;
    }
  };

  /**
   * Create the codebooks necessary for Huffman encoding and decoding given a
   * map of symbols to counts.
   *
   * @tparam MapType - an iterable associative array type that supports
   *                   iteration through key-value pairs.
   * @param counts_map - the map of symbols to counts.
   */
  template <typename MapType>
  void create_codebook_generic(const MapType& counts_map)
  {
    size_t current_length = 0;
    auto current_count = 0;
    auto lengths = sorted_lengths(get_code_lengths(make_tree(counts_map)));

    max_length_ = lengths.back().second;
    decoding_symbols_.reserve(lengths.size());
    decoding_indices_.reserve(max_length_);
    decoding_codes_.reserve(max_length_);

    for (auto pair: lengths) {
      auto symbol = pair.first;
      auto codeword_length = pair.second;

      decoding_symbols_.push_back(symbol);

      // Increment the codeword integer except on the first iteration.
      if (current_length != 0) {
        ++current_count;
      }

      // If the codewords increase in length, fill in the intermediate values
      // of the decoding indices and codes.
      if (current_length < codeword_length) {
        current_length = codeword_length;
        while (decoding_indices_.size() < codeword_length) {
          decoding_indices_.push_back(decoding_symbols_.size() - 1);
          current_count <<= 1;
          decoding_codes_.push_back(current_count);
        }
      }

      // Add the symbol-codeword pair to the encoding map.
      encoding_map_[symbol] = encode_int<int, BitVectorType>(current_count,
                                                             current_length);
    }
  }

  /**
   * Create a Huffman tree out of a mapping of keys to counts.
   */
  template <typename MapType>
  std::shared_ptr<HuffmanNode> make_tree(const MapType& counts)
  {
    using PtrType = std::shared_ptr<HuffmanNode>;
    std::priority_queue<PtrType, std::vector<PtrType>, HuffmanNodeComp> heap;
    for (auto it: counts) {
      heap.push(std::make_shared<HuffmanNode>(it.first, it.second));
    }
    while (heap.size() > 1) {
      auto first = heap.top();
      heap.pop();
      auto second = heap.top();
      heap.pop();
      heap.push(std::make_shared<HuffmanNode>(first, second));
    }
    return heap.top();
  }

  /**
   * Get the lengths of each encoded symbol in a Huffman tree.
   *
   * @param root - a pointer to the root of the Huffman tree.
   * @return - a map of symbols to their respective encoding lengths in bits.
   */
  std::unordered_map<Key, size_t> get_code_lengths(
    std::shared_ptr<HuffmanNode> root) const
  {
    std::unordered_map<Key, size_t> code_lengths;

    // Perform a breadth-first search through the Huffman tree to get the
    // length of each codeword.
    std::queue<std::pair<std::shared_ptr<HuffmanNode>,
                         size_t>> bfs_queue;
    bfs_queue.push(std::make_pair(root, 0));
    while (!bfs_queue.empty()) {
      std::pair<std::shared_ptr<HuffmanNode>,
                size_t> current_node = bfs_queue.front();
      bfs_queue.pop();
      if (current_node.first->symbol) {
        code_lengths.emplace(*(current_node.first->symbol),
                             current_node.second);
      } else {
        bfs_queue.push(std::make_pair(current_node.first->zero,
                                      current_node.second + 1));
        bfs_queue.push(std::make_pair(current_node.first->one,
                                      current_node.second + 1));
      }
    }

    return code_lengths;
  }

  /**
   * Sort a mapping of symbol-length pairs in order of length first and
   * return the resulting order.
   *
   * @param code_lengths - a mapping of symbols to their code length.
   * @return - a vector of symbol-length pairs sorted by length first, then
   *           symbol.
   */
  std::vector<std::pair<Key, size_t>> sorted_lengths(
    const std::unordered_map<Key, size_t>& code_lengths) const
  {
    std::vector<std::pair<Key, size_t>> codebook_vector{code_lengths.begin(),
                                                     code_lengths.end()};
    std::sort(codebook_vector.begin(), codebook_vector.end(), CodebookComp());
    return codebook_vector;
  }

  /**
   * Decode all or part of a codeword to get a symbol.
   *
   * @param codeword - a buffer of bits to decode.
   * @return - a symbol-integer pair, where the symbol is the first decoded
   *           symbol found in the codeword, and the integer is the number of
   *           bits in the encoding of this symbol.
   */
  std::pair<Key, int> decode_symbol(const BitVectorType& codeword) const
  {
    // If the integer represented by the bits is larger than the last element
    // in the decoding codes, then the codeword is of maximum length. Look up
    // the symbol corresponding to the offset from the symbol of maximum length.
    if (decode_int<int>(codeword) >= decoding_codes_.back()) {
      return std::make_pair(
        decoding_symbols_[decoding_indices_[max_length_ - 1]
                          + decode_int<int>(codeword)
                          - decoding_codes_[max_length_ - 1]],
        max_length_);
    }

    // Otherwise, perform a binary search in the decoding codes to determine
    // the length of the codeword. Define the initial low and high indices,
    // as well as helper functions to get the midpoint of the indices and the
    // amount by which we will shift the buffer to get the integer
    // encoding of the codeword of correct length.
    size_t lo = 0, hi = max_length_ - 1 ;
    const auto mid = [&lo, &hi]() { return lo + (hi - lo) / 2; };
    const auto shift_width = [this, &mid]() { return max_length_ - mid() - 1; };

    // Perform the binary search.
    while ((decode_int<int>(codeword) < decoding_codes_[mid()] << shift_width()
            || decode_int<int>(codeword)
               >= decoding_codes_[mid() + 1] << (shift_width() - 1))
           && lo != hi) {
      if (decode_int<int>(codeword) < decoding_codes_[mid()] << shift_width()) {
        hi = (hi == mid() ? hi - 1 : mid());
      } else {
        lo = (lo == mid() ? lo + 1 : mid());
      }
    }

    // Decode the buffer to get the symbol and return it along with the
    // codeword length.
    return std::make_pair(
      decoding_symbols_[decoding_indices_[mid()]
      + (decode_int<int>(codeword) >> shift_width()) - decoding_codes_[mid()]],
      mid() + 1);
  }

  std::unordered_map<Key, BitVectorType> encoding_map_;
  std::vector<Key> decoding_symbols_;
  std::vector<size_t> decoding_indices_;
  std::vector<int> decoding_codes_;
  size_t max_length_;
};

#endif //CAPS_HUFFMAN_H_
