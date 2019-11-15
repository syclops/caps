/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_HUFFMAN_H
#define CAPS_HUFFMAN_H

// Include C standard libraries.
#include <cstdlib>

// Include C++ standard libraries.
#include <optional>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

// Include other headers from this project.
#include "coder.h"
#include "delta_coder.h"
#include "string_coder.h"

// Include headers from other projects.

template<typename SymbolType, typename EncodingType>
class HuffmanCoder: public Coder<SymbolType, EncodingType>
{
 public:

  template<typename MapType>
  explicit HuffmanCoder(const MapType& counts)
    : encoding_map_{}, decoding_symbols_{}, decoding_indices_{},
      decoding_codes_{}, max_length_{0}
  {
    create_codebook_generic(counts);
  }

  inline bool valid_value(const SymbolType& value) const override
  {
    return encoding_map_.find(value) != encoding_map_.end();
  }

  inline size_t value_size(const SymbolType& value) const override
  {
    return encoding_map_.at(value).size();
  }

  EncodingType get_codebook(
    std::shared_ptr<Coder<SymbolType, EncodingType>> symbol_coder) const
  {
    EncodingType buffer;
    // Set up the encoders used.
    DeltaCoder<size_t, EncodingType> delta_size_coder;
    DeltaCoder<int, EncodingType> delta_int_coder;
    // Push number of symbols.
    delta_size_coder.encode(decoding_symbols_.size(), &buffer);
    // Push each self-delimiting symbol.
    auto buffer_before_symbols = buffer.size();
    for (const auto& symbol: decoding_symbols_) {
      symbol_coder->encode(symbol, &buffer);
    }
    auto buffer_after_symbols = buffer.size();
    std::cerr << "Encoding size of number of symbols: "
              << buffer_after_symbols - buffer_before_symbols << std::endl;
    // Push number of decoding indices.
    delta_size_coder.encode(decoding_indices_.size(), &buffer);
    // Push each decoding index.
    auto buffer_before_indices = buffer.size();
    for (const auto& index: decoding_indices_) {
      delta_size_coder.encode(index, &buffer);
    }
    auto buffer_after_indices = buffer.size();
    std::cerr << "Encoding size of number of indices: "
              << buffer_after_indices - buffer_before_indices << std::endl;
    // Push each decoding code.
    auto buffer_before_codes = buffer.size();
    for (const auto& code: decoding_codes_) {
      delta_int_coder.encode(code, &buffer);
    }
    auto buffer_after_codes = buffer.size();
    std::cerr << "Encoding size of number of codes: "
              << buffer_after_codes - buffer_before_codes << std::endl;
    return buffer;
  }


 protected:

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
    HuffmanNode(SymbolType symbol, int count)
      : symbol{std::shared_ptr<SymbolType>(new SymbolType(symbol))}, count{count}, size{1},
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
      : symbol{std::shared_ptr<SymbolType>()}, count{zero->count + one->count},
        size{zero->size + one->size}, zero{std::move(zero)}, one{std::move(one)}
    {
      // Nothing to do here.
    }

    std::shared_ptr<SymbolType> symbol;
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
    bool operator()(const std::pair<SymbolType, size_t> lhs,
                    const std::pair<SymbolType, size_t> rhs) const
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
    BinaryCoder<int, EncodingType> symbol_coder;
    auto lengths = sorted_lengths(get_code_lengths(make_tree(counts_map)));

    max_length_ = lengths.back().second;
    decoding_symbols_.reserve(lengths.size());
    decoding_indices_.reserve(max_length_);
    decoding_codes_.reserve(max_length_);

    std::cout<<max_length_<<std::endl;

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
        symbol_coder = BinaryCoder<int, EncodingType>{current_length};
        while (decoding_indices_.size() < codeword_length) {
          decoding_indices_.push_back(decoding_symbols_.size() - 1);
          current_count <<= 1;
          decoding_codes_.push_back(current_count);
        }
      }

      // Add the symbol-codeword pair to the encoding map.
      // TODO: make sure this doesn't leak memory or see if there is a safer
      //       way to do this.
      EncodingType* encoding = symbol_coder.encode(current_count);
      encoding_map_.emplace(symbol, *encoding);
      delete encoding;
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
//    auto unique_symbols = 0;  // debug
    for (auto it: counts) {
      const auto& [symbol, count] = it;
//      if (count == 1) {
//        ++unique_symbols;
//      }
      heap.push(std::make_shared<HuffmanNode>(symbol, count));
    }
//    std::cerr << unique_symbols << " unique symbols" << std::endl;
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
  std::unordered_map<SymbolType, size_t> get_code_lengths(
    std::shared_ptr<HuffmanNode> root) const
  {
    std::unordered_map<SymbolType, size_t> code_lengths;

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
  std::vector<std::pair<SymbolType, size_t>> sorted_lengths(
    const std::unordered_map<SymbolType, size_t>& code_lengths) const
  {
    std::vector<std::pair<SymbolType, size_t>> codebook_vector{code_lengths.begin(),
                                                        code_lengths.end()};
    std::sort(codebook_vector.begin(), codebook_vector.end(), CodebookComp());
    return codebook_vector;
  }

  void encode_impl(const SymbolType& value, EncodingType* encoding) const override
  {
    encoding->push_back(encoding_map_.at(value));
  }

  std::optional<std::pair<SymbolType, size_t>> decode_impl(
    const EncodingType& buffer, size_t position) const override
  {
    // Build a codeword from the selected position in the buffer.
    EncodingType codeword;
    codeword.reserve(max_length_);
    for (size_t i = 0; i < max_length_; ++i) {
      if (position + i >= buffer.size()) {
        codeword.resize(max_length_);
      } else {
        codeword.push_back(buffer[position + i]);
      }
    }

    // Attempt to decode the codeword as an integer. If this fails, decoding
    // fails as well.
    BinaryCoder<int, EncodingType> binary_coder(max_length_);
    auto decode_option = binary_coder.decode(codeword);
    if (!decode_option.has_value() || decode_option->second != max_length_) {
      return std::nullopt;
    }
    auto decode_int = decode_option->first;

    // If the integer represented by the bits is larger than the last element
    // in the decoding codes, then the codeword is of maximum length. Look up
    // the symbol corresponding to the offset from the symbol of maximum length.
    if (decode_int >= decoding_codes_.back()) {
      return std::make_pair(
        decoding_symbols_[decoding_indices_[max_length_ - 1] + decode_int
                          - decoding_codes_[max_length_ - 1]],
        max_length_);
    }

    // Otherwise, perform a binary search in the decoding codes to determine
    // the length of the codeword.
    auto index = binary_search(decode_int);

    // Decode the buffer to get the symbol and return it along with the
    // codeword length.
    return std::make_pair(
      decoding_symbols_[decoding_indices_[index]
                        + (decode_int >> (max_length_ - index - 1)) -
                        decoding_codes_[index]],
      index + 1);
  }

  size_t binary_search(int codeword) const
  {
    size_t lo = 0, hi = max_length_ - 1 ;
    const auto mid = [&lo, &hi]() { return lo + (hi - lo) / 2; };
    const auto shift_width = [this, &mid]() { return max_length_ - mid() - 1; };

    // Perform the binary search.
    while ((codeword < decoding_codes_[mid()] << shift_width()
            || codeword >= decoding_codes_[mid() + 1] << (shift_width() - 1))
           && lo != hi) {
      if (codeword < decoding_codes_[mid()] << shift_width()) {
        hi = (hi == mid() ? hi - 1 : mid());
      } else {
        lo = (lo == mid() ? lo + 1 : mid());
      }
    }
    return mid();
  }

  std::unordered_map<SymbolType, EncodingType> encoding_map_;
  std::vector<SymbolType> decoding_symbols_;
  std::vector<size_t> decoding_indices_;
  std::vector<int> decoding_codes_;
  size_t max_length_;
};

#endif //CAPS_HUFFMAN_H
