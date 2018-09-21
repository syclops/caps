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

inline std::vector<bool> encode_int(int value, int length)
{
  std::vector<bool> encoding;
  for (auto i = length; i > 0; --i) {
    encoding.push_back((1 << (i - 1) & value) != 0);
  }
  return encoding;
}

inline int decode_int(std::vector<bool> bits)
{
  int value = 0;
  for (auto b: bits) {
    value <<= 1;
    value += (b ? 1 : 0);
  }
  return value;
}

/**
 * Huffman encoder/decoder.
 *
 * This class provides functionality for constructing a Huffman encoder/decoder
 * based on a map of symbols (represented as STL strings) to counts (represented
 * as a number of times the symbol has occurred in an input sample). Upon
 * constructing such a tree, the class can then encode any symbol present in the
 * original input or decode any binary data representing symbols encoded with
 * the tree's codebook.
 *
 * The class's codebook is based on canonical Huffman encoding, which allows for
 * a compact representation of the codebook, both on disk and in memory. (The
 * compact in-memory representation is used for decoding, not encoding, and is
 * used in a separate class that handles this functionality.) This class also
 * allows dumping the codebook in a variety of forms, including as an STL
 * unordered map, ASCII, and binary.
 *
 * TODO: integrate Simon Gog's SDSL to allow a compact representation of the
 *       codebook.
 *
 * Note: this class assumes that all symbols to be encoded are composed entirely
 *       of ASCII characters (individual characters or strings). If this
 *       assumption is violated, the behavior of the encoder/decoder is
 *       undefined.
 *
 * TODO: determine whether STL strings support non-ASCII characters.
 *
 */
//class HuffmanCoder
//{
// public:
//
//  /**
//   * Construct a Huffman codebook from an ordered map.
//   * @param counts
//   */
//  explicit HuffmanCoder(const std::map<std::string, size_t>& counts);
//
//  /**
//   * Construct a Huffman codebook from an unordered map.
//   * @param counts
//   */
//  explicit HuffmanCoder(const std::unordered_map<std::string, size_t>& counts);
//
//  /**
//   * Write codebook to a map of binary bit encodings.
//   * @param map - a reference to the map in which to write the codebook.
//   */
//  std::unordered_map<std::string, std::vector<bool>> dump_codebook() const;
//
//  /**
//   * Get the lengths of the binary bit encodings for each symbol.
//   *
//   * Write for each symbol the length in bits of the encoding stored in the
//   * codebook.
//   *
//   * @param size_map - a reference to the map in which to store the association
//   *                   of symbols with bit lengths.
//   */
//  std::unordered_map<std::string, size_t> get_encoding_lengths() const;
//
//  /**
//   * Encode a symbol (in the form of a string) to a code word of bits.
//   * @param str
//   * @return
//   */
//  std::vector<bool> encode(std::string str) const;
//
//  /**
//   * Decode a code word to a string.
//   * @param codeword
//   * @return
//   */
//  std::string decode(const std::vector<bool>& codeword) const;
//
//  /**
//   * Dump the canonical codebook in binary form.
//   * @return
//   */
//  std::vector<bool> dump_codebook_bin() const;
//
////  /**
////   * Dump the canonical codebook in human-readable ASCII form.
////   *
////   * Write the information necessary to reconstruct the canonical Huffman
////   * codebook in ASCII form.
////   *
////   * @param out_stream - a reference to the output stream (stdout, a file
////   *                     stream, or a string stream) to which to write the
////   *                     codebook.
////   */
////  void dump_codebook_ascii(std::ostream& out_stream) const;
//
//  /**
//   * Get the size of the canonical Huffman codebook.
//   * @return
//   */
//  size_t size() const;
//
// protected:
//  struct HuffmanNode
//  {
//    /**
//     * Constructor from label and count.
//     * @param key
//     * @param count
//     */
//    HuffmanNode(std::string str, size_t count);
//
//    /**
//     * Constructor from two child nodes.
//     * @param zero
//     * @param one
//     */
//    HuffmanNode(std::shared_ptr<HuffmanNode> zero,
//                std::shared_ptr<HuffmanNode> one);
//
//    std::string symbol;
//    int count;
//    int size;
//    std::shared_ptr<HuffmanNode> zero;
//    std::shared_ptr<HuffmanNode> one;
//  };
//
//  /**
//   * Huffman node comparison functor.
//   *
//   * Compare two Huffman nodes to decide their relative ordering in the priority
//   * queue of nodes during Huffman tree construction. Given two Huffman node
//   * pointers, the functor returns true if the right-hand pointer comes later
//   * than the left-hand pointer in the queue. Given two pointers lhs and rhs,
//   * the pointer rhs will come after lhs in the priority queue if one of the
//   * following is true:
//   *
//   *   * the frequency count of rhs is higher than that of lhs,
//   *   * both lhs and rhs have the same frequency count and both contain just a
//   *     single symbol, and the symbol of rhs comes after that of lhs in a
//   *     lexicographic (dictionary) order,
//   *   * both lhs and rhs have the same frequency count, and rhs contains more
//   *     leaf nodes than lhs, or
//   *   * both lhs and rhs have the same frequency count and number of leaf
//   *     nodes, and lhs was inserted into the priority queue before rhs.
//   *
//   * Note: the std::priority_queue data structure considers the maximum element
//   *       to be the top of the queue. Because in Huffman tree construction we
//   *       want the smallest element (in terms of size and lexicographic order),
//   *       we simply reverse the direction of comparison in the comparator
//   *       functor (which is assumed to return true iff the left-hand pointer is
//   *       *less* than the right-hand pointer).
//   */
//  struct HuffmanNodeComp
//  {
//    /**
//     * Compare two Huffman nodes for ordering in the priority queue during tree
//     * construction.
//     *
//     * @param lhs - a pointer to the Huffman node on the left-hand side of the
//     *              operator.
//     * @param rhs - a pointer to the Huffman node on the right-hand side of the
//     *              operator.
//     */
//    bool operator()(const std::shared_ptr<HuffmanNode> lhs,
//                    const std::shared_ptr<HuffmanNode> rhs) const;
//  };
//
//  struct CodebookComp
//  {
//    bool operator()(const std::pair<std::string, size_t> lhs,
//                    const std::pair<std::string, size_t> rhs) const;
//  };
//
//  /**
//   * Helper for create_codebook that takes a generic map (ordered or unordered).
//   * @tparam MapType - a type that supports iteration over its key-value pairs.
//   * @param counts_map - the map of symbols to counts over which to iterate.
//   */
//  template <typename MapType>
//  void create_codebook_generic(const MapType& counts_map)
//  {
//    size_t current_length = 0;
//    size_t current_count = 0;
//    for (auto symbol_length: sorted_lengths(get_code_lengths(make_tree(
//                                                               counts_map)))) {
//      if (current_length != 0) {
//        ++current_count;
//      }
//      if (current_length != symbol_length.second) {
//        current_length = symbol_length.second;
//      }
//      encoding_map_[symbol_length.first] = encode_int(current_count,
//                                                      current_length);
//    }
//  }
//
//  /**
//   * Construct a Huffman tree given a collection of symbol counts.
//   *
//   * Construct a Huffman tree given a collection of (symbol, count) pairs
//   * representing the number of occurrences of symbols in a given corpus of
//   * text. The Huffman tree is constructed in a way that minimizes the length of
//   * each symbol's encoding.
//   *
//   * @tparam PairItrType - a collection of pairs that supports iteration in a
//   *                       range-based for loop.
//   * @param counts - a collection of symbol counts, with each count represented
//   *                 as a pair `(symbol, count)`.
//   * @return - a shared pointer to the root node of the Huffman tree.
//   */
//  template <typename PairItrType>
//  std::shared_ptr<HuffmanNode> make_tree(const PairItrType& counts)
//  {
//    std::priority_queue<std::shared_ptr<HuffmanNode>,
//                        std::vector<std::shared_ptr<HuffmanNode>>,
//                        HuffmanNodeComp> heap;
//    for (auto it: counts) {
//      heap.push(std::make_shared<HuffmanNode>(it.first, it.second));
//    }
//    while (heap.size() > 1) {
//      auto first = heap.top();
//      heap.pop();
//      auto second = heap.top();
//      heap.pop();
//      heap.push(std::make_shared<HuffmanNode>(second, first));
//    }
//    return heap.top();
//  }
//
//  std::unordered_map<std::string, int> get_code_lengths(
//    std::shared_ptr<HuffmanNode> root) const;
//
//  void write_char(char c, std::vector<bool>& bitstream) const;
//
//  std::vector<std::pair<std::string, int>> sorted_lengths(
//    const std::unordered_map<std::string, int>& code_lengths) const;
//
//  std::unordered_map<std::string, std::vector<bool>> encoding_map_;
//  std::vector<std::string> decoding_symbols_;
//  std::vector<int> decoding_indices_;
//  std::vector<int> decoding_codes_;
//};

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
 * This class takes a list of counts and
 *
 * @tparam Key - the type of symbols to encode using Huffman coding.
 */
template <typename Key>
class HuffmanCoder
{
 public:

  explicit HuffmanCoder(const std::map<Key, int>& counts)
    : encoding_map_{}, decoding_symbols_{}, decoding_indices_{},
      decoding_codes_{}, max_length_{0}
  {
    create_codebook_generic(counts);
  }

  explicit HuffmanCoder(const std::unordered_map<Key, int>& counts)
    : encoding_map_{}, decoding_symbols_{}, decoding_indices_{},
      decoding_codes_{}, max_length_{0}
  {
    create_codebook_generic(counts);
  }

  std::unordered_map<Key, std::vector<bool>> encoding_map() const
  {
    return encoding_map_;
  }

  std::tuple<std::vector<Key>, std::vector<int>, std::vector<int>>
  decoding_map() const
  {
    return std::make_tuple(decoding_symbols_, decoding_indices_,
                           decoding_codes_);
  };

  std::vector<bool> encode(typename symbol_sequence<Key>::type sequence) const
  {
    std::vector<bool> encoded;
    symbol_sequence<Key> symbols{sequence};
    for (auto symbol: symbols.sequence) {
      auto encoded_symbol = encode_symbol(symbol);
      encoded.reserve(encoded.size() + encoded_symbol.size());
      encoded.insert(encoded.end(), encoded_symbol.begin(),
                     encoded_symbol.end());
    }
    return encoded;
  }

  typename symbol_sequence<Key>::type decode(
    const std::vector<bool>& codeword) const
  {
    size_t index = 0;
    symbol_sequence<Key> decoded;
//    auto codeword_length = [&index, &codeword, this]() {
//      return std::min(max_length_, codeword.size() - index);
//    };
    while (codeword.size() - index > 0) {
      std::vector<bool> current_bits(max_length_);
      for (size_t i = 0; i < max_length_; ++i) {
        if (index + i >= codeword.size()) {
          current_bits[i] = false;
        } else {
          current_bits[i] = codeword[index + i];
        }
      }
      auto decoded_symbol = decode_symbol(current_bits);
      decoded.append(decoded_symbol.first);
      index += decoded_symbol.second;
//      std::cout << "Decoded ";
//      for (auto b: current_bits) {
//        std::cout << (b ? '1' : '0');
//      }
//      std::cout << " to " << decoded_symbol.first << " and consumed "
//                << decoded_symbol.second << " bits" << std::endl;
    }
    return decoded.sequence;
  }

 private:
  struct HuffmanNode
  {
    HuffmanNode(Key symbol, int count)
      : symbol{std::shared_ptr<Key>(new Key(symbol))}, count{count}, size{1}, zero{}, one{}
    {
      // Nothing to do here.
    }

    HuffmanNode(std::shared_ptr<HuffmanNode> zero,
                std::shared_ptr<HuffmanNode> one)
      : symbol{std::shared_ptr<Key>()}, count{zero->count + one->count}, size{zero->size + one->size},
        zero{std::move(zero)}, one{std::move(one)}
    {
      // Nothing to do here.
    }

    std::shared_ptr<Key> symbol;
    int count;
    int size;
    std::shared_ptr<HuffmanNode> zero;
    std::shared_ptr<HuffmanNode> one;
  };

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

  template <typename MapType>
  void create_codebook_generic(const MapType& counts_map)
  {
    auto current_length = 0;
    auto current_count = 0;
    auto lengths = sorted_lengths(get_code_lengths(make_tree(counts_map)));
    decoding_symbols_.resize(lengths.size());
    decoding_indices_.resize(lengths.rbegin()->second);
    decoding_codes_.resize(lengths.rbegin()->second);
    for (auto i = 0; i < static_cast<int>(lengths.size()); ++i) {
      decoding_symbols_[i] = lengths[i].first;
      if (current_length != 0) {
        ++current_count;
      }
      if (current_length != lengths[i].second) {
        if (current_length != 0) {
          current_count <<= lengths[i].second - current_length;
        }
        for (auto j = current_length; j < lengths[i].second; ++j) {
          decoding_indices_[j] = i;
          decoding_codes_[j] = current_count >> (lengths[i].second - j - 1);
        }
        current_length = lengths[i].second;
      }
      encoding_map_[lengths[i].first] = encode_int(current_count,
                                                      current_length);
    }
    max_length_ = current_length;
  }

  template <typename PairItrType>
  std::shared_ptr<HuffmanNode> make_tree(const PairItrType& counts)
  {
    std::priority_queue<std::shared_ptr<HuffmanNode>,
                        std::vector<std::shared_ptr<HuffmanNode>>,
                        HuffmanNodeComp> heap;
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

  std::unordered_map<Key, int> get_code_lengths(
    std::shared_ptr<HuffmanNode> root) const
  {
    std::unordered_map<Key, int> code_lengths;
    std::queue<std::pair<std::shared_ptr<HuffmanNode>, int>> bfs_queue;
    bfs_queue.push(std::make_pair(root, 0));
    while (!bfs_queue.empty()) {
      std::pair<std::shared_ptr<HuffmanNode>, int> current_node = bfs_queue.front();
      bfs_queue.pop();
      if (current_node.first->symbol) {
        code_lengths.emplace(*(current_node.first->symbol), current_node.second);
      } else {
        bfs_queue.push(std::make_pair(current_node.first->zero,
                                      current_node.second + 1));
        bfs_queue.push(std::make_pair(current_node.first->one,
                                      current_node.second + 1));
      }
    }
    return code_lengths;
  }

  std::vector<std::pair<Key, int>> sorted_lengths(
    const std::unordered_map<Key, int>& code_lengths) const
  {
    std::vector<std::pair<Key, int>> codebook_vector{code_lengths.begin(),
                                                             code_lengths.end()};
    std::sort(codebook_vector.begin(), codebook_vector.end(), CodebookComp());
    return codebook_vector;
  }

  std::vector<bool> encode_symbol(Key symbol) const
  {
    return encoding_map_.at(symbol);
  }

  std::pair<Key, int> decode_symbol(const std::vector<bool>& codeword) const
  {
    if (decode_int(codeword) >= *decoding_codes_.rbegin()) {
      return std::make_pair(
        decoding_symbols_[decoding_indices_[max_length_ - 1] + decode_int(codeword)
                          - decoding_codes_[max_length_ - 1]],
        max_length_);
    }
    size_t lo = 0, hi = max_length_ - 1 ;
    const auto mid = [&lo, &hi]() { return lo + (hi - lo) / 2; };
    const auto shift_width = [this, &mid]() { return max_length_ - mid() - 1; };
    while ((decode_int(codeword) < decoding_codes_[mid()] << shift_width()
            || decode_int(codeword)
               >= decoding_codes_[mid() + 1] << (shift_width() - 1))
           && lo != hi) {
      if (decode_int(codeword) < decoding_codes_[mid()] << shift_width()) {
        hi = (hi == mid() ? hi - 1 : mid());
      } else {
        lo = (lo == mid() ? lo + 1 : mid());
      }
    }
    return std::make_pair(decoding_symbols_[decoding_indices_[mid()]
                                            + (decode_int(codeword) >> shift_width())
                                            - decoding_codes_[mid()]],
                          mid() + 1);
  }

  std::unordered_map<Key, std::vector<bool>> encoding_map_;
  std::vector<Key> decoding_symbols_;
  std::vector<int> decoding_indices_;
  std::vector<int> decoding_codes_;
  size_t max_length_;
};

#endif //CAPS_HUFFMAN_H_
