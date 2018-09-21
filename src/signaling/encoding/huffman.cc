#include "huffman.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <boost/bimap.hpp>

HuffmanCoder::HuffmanCoder(const std::map<std::string, size_t>& counts)
  : encoding_map_{}, decoding_symbols_{}, decoding_indices_{}, decoding_codes_{}
{
  create_codebook_generic(counts);
}

HuffmanCoder::HuffmanCoder(const std::unordered_map<std::string, size_t>& counts)
  : encoding_map_{}, decoding_symbols_{}, decoding_indices_{}, decoding_codes_{}
{
  create_codebook_generic(counts);
}

std::unordered_map<std::string, std::vector<bool>> HuffmanCoder::dump_codebook(
  ) const
{
  std::unordered_map<std::string, std::vector<bool>> codebook;
  for (auto it: codebook_.left) {
    codebook[it.first] = it.second;
  }
  return codebook;
}

std::unordered_map<std::string, size_t> HuffmanCoder::get_encoding_lengths(
  ) const
{
  std::unordered_map<std::string, size_t> size_map;
  for (auto it: codebook_.left) {
    size_map[it.first] = it.second.size();
  }
  return size_map;
}

std::vector<bool> HuffmanCoder::encode(const std::string str) const
{
  return encoding_map_.at(str);
}

std::string HuffmanCoder::decode(const std::vector<bool>& codeword) const
{
  // TODO
  return "";
}

std::vector<bool> HuffmanCoder::dump_codebook_bin() const
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

//void HuffmanCoder::dump_codebook_ascii(std::ostream& out_stream) const
//{
//  size_t length = 0;
//  for (auto pair: sorted_codebook_lengths()) {
//    out_stream << pair.first;
//    if (length != pair.second) {
//      out_stream << " " << pair.second;
//    }
//    out_stream << std::endl;
//  }
//}

void HuffmanCoder::write_char(char c, std::vector<bool>& bitstream) const
{
  for (size_t i = 8; i > 0; --i) {
    bitstream.push_back(((c >> (i - 1)) & 0x1) != 0);
  }
}

size_t HuffmanCoder::size() const
{
  return codebook_size_;
}

HuffmanCoder::HuffmanNode::HuffmanNode(std::string str, int count)
  : symbol{std::move(str)}, count{count}, size{1}, zero{}, one{}
{
  // Nothing to do here.
}

HuffmanCoder::HuffmanNode::HuffmanNode(std::shared_ptr<HuffmanNode> zero,
                                      std::shared_ptr<HuffmanNode> one)
  : symbol{}, count{zero->count + one->count}, size{zero->size + one->size},
    zero{zero}, one{one}
{
  // Nothing to do here.
}

bool HuffmanCoder::HuffmanNodeComp::operator()(
  const std::shared_ptr<HuffmanNode> lhs,
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

bool HuffmanCoder::CodebookComp::operator()(
  const std::pair<std::string, int> lhs,
  const std::pair<std::string, int> rhs) const
{
  if (lhs.second == rhs.second) {
    return lhs.first > rhs.first;
  }
  return lhs.second > rhs.second;
}

std::unordered_map<std::string, int> HuffmanCoder::get_code_lengths(
  std::shared_ptr<HuffmanNode> root) const
{
  std::unordered_map<std::string, int> code_lengths;
  std::queue<std::pair<std::shared_ptr<HuffmanNode>, int>> bfs_queue;
  bfs_queue.push(std::make_pair(root, 0));
  while (!bfs_queue.empty()) {
    std::pair<std::shared_ptr<HuffmanNode>, int> current_node = bfs_queue.front();
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
  return code_lengths;
}

std::vector<std::pair<std::string, int>> HuffmanCoder::sorted_lengths(
  const std::unordered_map<std::string, int>& code_lengths) const
{
  std::vector<std::pair<std::string, int>> codebook_vector{code_lengths.begin(),
                                                           code_lengths.end()};
  std::sort(codebook_vector.begin(), codebook_vector.end(), CodebookComp);
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
