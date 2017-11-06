#include "huffman.h"
#include <cstdlib>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <boost/bimap.hpp>

HuffmanTree::HuffmanTree(const std::map<std::string, size_t>& counts)
  : codebook_(), codebook_size_(0)
{
  create_codebook_generic(counts);
}

HuffmanTree::HuffmanTree(const std::unordered_map<std::string, size_t>& counts)
  : codebook_(), codebook_size_(0)
{
  create_codebook_generic(counts);
}

HuffmanTree::HuffmanTree(HuffmanTree &&orig) noexcept
{
  swap(*this, orig);
}

HuffmanTree& HuffmanTree::operator=(HuffmanTree rhs)
{
  swap(*this, rhs);
  return *this;
}

void swap(HuffmanTree& first, HuffmanTree& second)
{
  using std::swap;
  swap(first.codebook_, second.codebook_);
  swap(first.codebook_size_, second.codebook_size_);
}

std::unordered_map<std::string, std::vector<bool>> HuffmanTree::dump_codebook(
  ) const
{
  std::unordered_map<std::string, std::vector<bool>> codebook;
  for (auto it: codebook_.left) {
    codebook[it.first] = it.second;
  }
  return codebook;
}

std::unordered_map<std::string, size_t> HuffmanTree::get_encoding_lengths(
  ) const
{
  std::unordered_map<std::string, size_t> size_map;
  for (auto it: codebook_.left) {
    size_map[it.first] = it.second.size();
  }
  return size_map;
}

std::vector<bool> HuffmanTree::encode(const std::string str) const
{
  return codebook_.left.at(str);
}

std::string HuffmanTree::decode(const std::vector<bool>& codeword) const
{
  return codebook_.right.at(codeword);
}

std::vector<bool> HuffmanTree::dump_codebook_bin() const
{
  std::vector<bool> bitstream;
  for (auto it: codebook_.left) {
    for (char c: it.first) {
      write_char(c, bitstream);
    }
    for (bool b: it.second) {
      bitstream.push_back(b);
    }
  }
  return bitstream;
}

void HuffmanTree::dump_codebook_ascii(std::ostream& out_stream) const
{
  size_t length = 0;
  for (auto pair: sorted_codebook_lengths()) {
    out_stream << pair.first;
    if (length != pair.second) {
      out_stream << " " << pair.second;
    }
    out_stream << std::endl;
  }
}

void HuffmanTree::write_char(char c, std::vector<bool>& bitstream) const
{
  for (size_t i = 8; i > 0; --i) {
    bitstream.push_back(((c >> (i - 1)) & 0x1) != 0);
  }
}

size_t HuffmanTree::size() const
{
  return codebook_size_;
}

HuffmanTree::HuffmanNode::HuffmanNode(std::string str, size_t count)
  : symbol(str), count(count), size(1), zero(nullptr), one(nullptr)
{
  // Nothing to do here.
}

HuffmanTree::HuffmanNode::HuffmanNode(HuffmanNode* zero, HuffmanNode* one)
  : symbol(""), count(zero->count + one->count), size(zero->size + one->size),
    zero(zero), one(one)
{
  // Nothing to do here.
}

HuffmanTree::HuffmanNode::HuffmanNode(const HuffmanNode& orig)
  : symbol(orig.symbol), count(orig.count)
{
  zero = new HuffmanNode(*orig.zero);
  one = new HuffmanNode(*orig.one);
}

HuffmanTree::HuffmanNode::HuffmanNode(HuffmanNode&& orig) noexcept
{
  swap(*this, orig);
}

typename HuffmanTree::HuffmanNode& HuffmanTree::HuffmanNode::operator=(HuffmanNode rhs)
{
  swap(*this, rhs);
  return *this;
}

HuffmanTree::HuffmanNode::~HuffmanNode()
{
  delete zero;
  delete one;
}

void swap(HuffmanTree::HuffmanNode &first, HuffmanTree::HuffmanNode &second)
{
  using std::swap;
  swap(first.symbol, second.symbol);
  swap(first.count, second.count);
  swap(first.zero, second.zero);
  swap(first.one, second.one);
}

bool HuffmanTree::HuffmanNodeComp::operator()(const HuffmanNode *lhs,
                                              const HuffmanNode *rhs) const
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

bool HuffmanTree::CodebookComp::operator()(
  const std::pair<std::string, size_t> lhs,
  const std::pair<std::string, size_t> rhs) const
{
  if (lhs.second == rhs.second) {
    return lhs.first > rhs.first;
  }
  return lhs.second > rhs.second;
}

std::unordered_map<std::string, size_t> HuffmanTree::get_code_lengths(
  HuffmanNode* root) const
{
  std::unordered_map<std::string, size_t> code_lengths;
  std::queue<std::pair<HuffmanNode*, size_t>> bfs_queue;
  bfs_queue.push(std::make_pair(root, 0));
  while (!bfs_queue.empty()) {
    std::pair<HuffmanNode*, size_t> current_node = bfs_queue.front();
    bfs_queue.pop();
    if (current_node.first->symbol.empty()) {
      bfs_queue.push(std::make_pair(current_node.first->zero,
                                    current_node.second + 1));
      bfs_queue.push(std::make_pair(current_node.first->one,
                                    current_node.second + 1));
    } else {
      code_lengths[current_node.first->symbol] = current_node.second;
    }
  }
  delete root;
  return code_lengths;
}

std::vector<std::pair<std::string, size_t>> HuffmanTree::sorted_lengths(
  const std::unordered_map<std::string, size_t>& code_lengths) const
{
  std::priority_queue<std::pair<std::string, size_t>,
    std::vector<std::pair<std::string, size_t>>,
    CodebookComp> heap;
  for (auto it: code_lengths) {
    heap.push(it);
  }
  std::vector<std::pair<std::string, size_t>> codebook_vector;
  while (!heap.empty()) {
    codebook_vector.push_back(heap.top());
    heap.pop();
  }
  return codebook_vector;
}

std::vector<std::pair<std::string, size_t>>
  HuffmanTree::sorted_codebook_lengths() const
{
  std::priority_queue<std::pair<std::string, size_t>,
                      std::vector<std::pair<std::string, size_t>>,
                      CodebookComp> heap;
  for (auto it: codebook_.left) {
    heap.push(std::make_pair(it.first, it.second.size()));
  }
  std::vector<std::pair<std::string, size_t>> codebook_vector;
  while (!heap.empty()) {
    codebook_vector.push_back(heap.top());
    heap.pop();
  }
  return codebook_vector;
}

std::vector<bool> encode_int(size_t value, size_t length)
{
  std::vector<bool> encoding;
  for (size_t i = length; i > 0; --i) {
    encoding.push_back((1 << (i - 1) & value) != 0);
  }
  return encoding;
}
