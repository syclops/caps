/**
 * Huffman encoding and decoding functions.
 *
 * Author: Steve Matsumoto
 *
 * TODO: License information
 */

#ifndef CAPS_HUFFMAN_H_
#define CAPS_HUFFMAN_H_

#include <cstdlib>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>
#include <boost/bimap.hpp>

std::vector<bool> encode_int(size_t value, size_t length);

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
class HuffmanTree
{
 public:

  /**
   * Construct a Huffman codebook from an ordered map.
   * @param counts
   */
  HuffmanTree(const std::map<std::string, size_t>& counts);

  /**
   * Construct a Huffman codebook from an unordered map.
   * @param counts
   */
  HuffmanTree(const std::unordered_map<std::string, size_t>& counts);

  /**
   * Copy constructor.
   * @param orig - the original instance of HuffmanTree to make a copy of.
   */
  HuffmanTree(HuffmanTree& orig) = default;

  /**
   * Move constructor.
   * @param orig - the original instance of HuffmanTree to move.
   */
  HuffmanTree(HuffmanTree&& orig) noexcept;

  /**
   * Assignment operator.
   * @param rhs
   * @return
   */
  HuffmanTree& operator=(HuffmanTree rhs);

  /**
   * Destructor.
   */
  virtual ~HuffmanTree() = default;

  /**
   * Swap two instances of a HuffmanTree.
   * @param first
   * @param second
   */
  friend void swap(HuffmanTree& first, HuffmanTree& second);

  /**
   * Write codebook to a map of binary bit encodings.
   * @param map - a reference to the map in which to write the codebook.
   */
  std::unordered_map<std::string, std::vector<bool>> dump_codebook() const;

  /**
   * Get the lengths of the binary bit encodings for each symbol.
   *
   * Write for each symbol the length in bits of the encoding stored in the
   * codebook.
   *
   * @param size_map - a reference to the map in which to store the association
   *                   of symbols with bit lengths.
   */
  std::unordered_map<std::string, size_t> get_encoding_lengths() const;

  /**
   * Encode a symbol (in the form of a string) to a code word of bits.
   * @param str
   * @return
   */
  std::vector<bool> encode(std::string str) const;

  /**
   * Decode a code word to a string.
   * @param codeword
   * @return
   */
  std::string decode(const std::vector<bool>& codeword) const;

  /**
   * Dump the canonical codebook in binary form.
   * @return
   */
  std::vector<bool> dump_codebook_bin() const;

  /**
   * Dump the canonical codebook in human-readable ASCII form.
   *
   * Write the information necessary to reconstruct the canonical Huffman
   * codebook in ASCII form.
   *
   * @param out_stream - a reference to the output stream (stdout, a file
   *                     stream, or a string stream) to which to write the
   *                     codebook.
   */
  void dump_codebook_ascii(std::ostream& out_stream) const;

  /**
   * Get the size of the canonical Huffman codebook.
   * @return
   */
  size_t size() const;

 protected:
  struct HuffmanNode
  {
    /**
     * Constructor from label and count.
     * @param key
     * @param count
     */
    HuffmanNode(std::string str, size_t count);

    /**
     * Constructor from two child nodes.
     * @param zero
     * @param one
     */
    HuffmanNode(HuffmanNode* zero, HuffmanNode* one);

    /**
     * Copy constructor.
     * @param orig
     */
    HuffmanNode(const HuffmanNode& orig);

    /**
     * Move constructor.
     * @param orig
     */
    HuffmanNode(HuffmanNode&& orig) noexcept;

    /**
     * Copy and move assignment operator.
     * @param rhs
     * @return
     */
    HuffmanNode& operator=(HuffmanNode rhs);

    /**
     * Destructor.
     */
    ~HuffmanNode();

    std::string symbol;
    size_t count;
    size_t size;
    HuffmanNode* zero;
    HuffmanNode* one;
  };

  /**
   * Swap two instances of a HuffmanNode.
   * @param first
   * @param second
   */
  friend void swap(HuffmanNode& first, HuffmanNode& second);

  /**
   * Huffman node comparison functor.
   *
   * Compare two Huffman nodes to decide their relative ordering in the priority
   * queue of nodes during Huffman tree construction. Given two Huffman node
   * pointers, the functor returns true if the right-hand pointer comes later
   * than the left-hand pointer in the queue. Given two pointers lhs and rhs,
   * the pointer rhs will come after lhs in the priority queue if one of the
   * following is true:
   *
   *   * the frequency count of rhs is higher than that of lhs,
   *   * both lhs and rhs have the same frequency count and both contain just a
   *     single symbol, and the symbol of rhs comes after that of lhs in a
   *     lexicographic (dictionary) order,
   *   * both lhs and rhs have the same frequency count, and rhs contains more
   *     leaf nodes than lhs, or
   *   * both lhs and rhs have the same frequency count and number of leaf
   *     nodes, and lhs was inserted into the priority queue before rhs.
   *
   * Note: the std::priority_queue data structure considers the maximum element
   *       to be the top of the queue. Because in Huffman tree construction we
   *       want the smallest element (in terms of size and lexicographic order),
   *       we simply reverse the direction of comparison in the comparator
   *       functor (which is assumed to return true iff the left-hand pointer is
   *       *less* than the right-hand pointer).
   */
  struct HuffmanNodeComp
  {
    /**
     * Compare two Huffman nodes for ordering in the priority queue during tree
     * construction.
     *
     * @param lhs - a pointer to the Huffman node on the left-hand side of the
     *              operator.
     * @param rhs - a pointer to the Huffman node on the right-hand side of the
     *              operator.
     */
    bool operator()(const HuffmanNode* lhs, const HuffmanNode* rhs) const;
  };

  struct CodebookComp
  {
    bool operator()(const std::pair<std::string, size_t> lhs,
                    const std::pair<std::string, size_t> rhs) const;
  };

  /**
   * Helper for create_codebook that takes a generic map (ordered or unordered).
   * @tparam MapType - a type that supports iteration over its key-value pairs.
   * @param counts_map - the map of symbols to counts over which to iterate.
   */
  template <typename MapType>
  void create_codebook_generic(const MapType& counts_map)
  {
    size_t current_length = 0;
    size_t current_count = 0;
    HuffmanNode* root = make_tree(counts_map);
    std::unordered_map<std::string, size_t> lengths = get_code_lengths(root);

    for (std::pair<std::string, size_t> symbol_length: sorted_lengths(lengths)) {
      if (current_length != 0) {
        ++current_count;
      }
      if (current_length != symbol_length.second) {
        current_length = symbol_length.second;
      }
      codebook_.insert(boost::bimap<std::string, std::vector<bool>>::value_type(
        symbol_length.first, encode_int(current_count, current_length)));
    }
  }

  template <typename MapType>
  HuffmanNode* make_tree(const MapType& counts_map)
  {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>,
      HuffmanNodeComp> heap;
    for (auto it: counts_map) {
      heap.push(new HuffmanNode(it.first, it.second));
    }
    while (heap.size() > 1) {
      HuffmanNode* first = heap.top();
      heap.pop();
      HuffmanNode* second = heap.top();
      heap.pop();
      heap.push(new HuffmanNode(second, first));
    }
    return heap.top();
  }

  std::unordered_map<std::string, size_t> get_code_lengths(
    HuffmanNode* root) const;

  void write_char(char c, std::vector<bool>& bitstream) const;

  std::vector<std::pair<std::string, size_t>> sorted_lengths(
    const std::unordered_map<std::string, size_t>& code_lengths) const;

  /**
   * Produce a codebook sorted by length first and alphabetical order second.
   *
   * @return - a vector of string-integer pairs representing the codebook
   *           symbols along with their encoded lengths. The integers will be in
   *           nondecreasing order, and for a given value of an integer, the
   *           symbols will appear in lexicographic order.
   */
  std::vector<std::pair<std::string, size_t>> sorted_codebook_lengths() const;

  boost::bimap<std::string, std::vector<bool>> codebook_;
  size_t codebook_size_;
};

#endif //CAPS_HUFFMAN_H_
