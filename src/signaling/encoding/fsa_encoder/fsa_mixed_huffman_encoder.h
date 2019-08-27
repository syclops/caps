/**
 * FSA Mixed Huffman Encoder
 * Author: Yucheng Dai <yuchengd@andrew.cmu.edu>
 */

#ifndef CAPS_FSA_MIXED_HUFFMAN_ENCODER_H
#define CAPS_FSA_MIXED_HUFFMAN_ENCODER_H

// Include C++ standard libraries.
#include <memory>
#include <string>

// Include other headers from this project.
#include "../../lexicon/fsa_lexicon/fsa_lexicon.h"
#include "../coder/mixed_huffman_coder.h"
#include "../coder/char_huffman_coder.h"
#include "../coder/huffman_coder.h"
#include "../coder/signed_int_coder.h"
#include "fsa_encoder.h"

template <typename BitVectorType>
class FSAMixedHuffmanEncoder: public FSAEncoder<BitVectorType>
{
 public:

  FSAMixedHuffmanEncoder() = delete;

  explicit FSAMixedHuffmanEncoder(const FSALexicon& lexicon)
    : FSAEncoder<BitVectorType>{lexicon}
  {
    FSAEncoder<BitVectorType>::order_nodes();
    auto diff_counts = get_ordering_diff_counts(lexicon);
    auto destination_coder = std::make_shared<HuffmanCoder<int, BitVectorType>>(
      diff_counts);
    using DestCoderType = Coder<int, BitVectorType>;
    FSAEncoder<BitVectorType>::destination_coder_ =
      std::static_pointer_cast<DestCoderType>(destination_coder);

    auto [counts1, counts2] = get_new_counts(get_label_counts(lexicon));
    auto temp_char_coder = std::make_shared<CharHuffmanCoder<std::string,
                                                     BitVectorType>>(counts2);
    auto temp_string_coder = std::make_shared<HuffmanCoder<std::string,
                                                     BitVectorType>>(counts1);
    LabeledGraph::LabelMap counts3{};
//  size_t cnt=0, saved=0;
    for (auto& [symbol, count]: counts1){
      if (temp_char_coder->value_size(symbol)>temp_string_coder->value_size(symbol)){
        counts3.emplace(symbol, count);
      }else{
        for(auto c: symbol){
          if (counts2.find(c)!=counts2.end()) counts2.at(c) += count;
          else counts2.emplace(c, count);
        }
        if (counts2.find(END_OF_STRING)!=counts2.end()) counts2.at(END_OF_STRING)+=count;
        else counts2.emplace(END_OF_STRING, count);
//      cnt++;
//      saved+=(temp_string_coder->value_size(symbol)-temp_char_coder->value_size(symbol))*count;
      }
    }
//  std::cout<<"cnt: "<<cnt<<std::endl;
//  std::cout<<"saved: "<<saved<<std::endl;
    auto label_coder = std::make_shared<MixedHuffmanCoder<std::string,
                                                     BitVectorType>>(counts3, counts2);
    using LabelCoderType = Coder<std::string, BitVectorType>;
    FSAEncoder<BitVectorType>::label_coder_ =
      std::static_pointer_cast<LabelCoderType>(label_coder);
  }

 protected:

  void add_prefix(BitVectorType& buffer) override
  {
//    std::cerr << "Adding prefix" << std::endl;
    // Define type aliases.
    using LabelType = std::string;
    using LabelAbstractType = Coder<LabelType, BitVectorType>;
    using LabelCoderType = StringCoder<LabelType, BitVectorType>;
    using LabelHuffmanType = MixedHuffmanCoder<LabelType, BitVectorType>;
    using DestType = int;
    using DestAbstractType = Coder<DestType, BitVectorType>;
    using DestCoderType = SignedIntCoder<DestType, BitVectorType>;
    using DestBaseCoderType = DeltaCoder<DestType, BitVectorType>;
    using DestHuffmanType = HuffmanCoder<DestType, BitVectorType>;

    auto string_coder = std::make_shared<LabelCoderType>();
    auto label_codebook = std::static_pointer_cast<LabelHuffmanType>(
        FSAEncoder<BitVectorType>::label_coder_)->get_codebook(
          std::static_pointer_cast<LabelAbstractType>(string_coder));
//  std::cout << label_codebook.size() << std::endl;
    buffer.push_back(label_codebook);

    auto dest_coder = std::make_shared<DestCoderType>(new DestBaseCoderType);
    auto dest_codebook = std::static_pointer_cast<DestHuffmanType>(
        FSAEncoder<BitVectorType>::destination_coder_)->get_codebook(
          std::static_pointer_cast<DestAbstractType>(dest_coder));
//  std::cout << dest_codebook.size() << std::endl;
    buffer.push_back(dest_codebook);
  }

  const LabeledGraph::LabelMap& get_label_counts(
    const FSALexicon& lexicon) const
  {
    return const_cast<const LabeledGraph::LabelMap&>(lexicon.get_graph().get_label_counts());
  }

  std::unordered_map<int, int> get_ordering_diff_counts(
    const FSALexicon& lexicon) const
  {
    std::unordered_map<int, int> diff_counts;
    for (const auto& source: lexicon.get_graph().get_nodes()) {
      for (const auto& [label, child]: source->get_out_edges()) {
        auto diff = FSAEncoder<BitVectorType>::node_to_order_.at(child)
          - FSAEncoder<BitVectorType>::node_to_order_.at(source.get());
        diff = diff < 0 ? -diff : diff;
        if (diff_counts.find(diff) == diff_counts.end()) {
          diff_counts[diff] = 1;
        } else {
          ++diff_counts[diff];
        }
      }
    }
    return diff_counts;
  }

  std::pair<LabeledGraph::LabelMap, std::unordered_map<char, size_t>> get_new_counts(const LabeledGraph::LabelMap& counts) const{
    LabeledGraph::LabelMap counts1;
    std::unordered_map<char, size_t> counts2;
    for (const auto& [symbol, count]: counts){
      if (/*symbol.length()!=1 && count>3 && symbol.length()<=30*/
        count!=1) counts1.emplace(symbol, count);
      else{
        for(auto c: symbol){
          if (counts2.find(c)!=counts2.end()) counts2.at(c) += count;
          else counts2.emplace(c, count);
        }
        if (counts2.find(END_OF_STRING)!=counts2.end()) counts2.at(END_OF_STRING)+=count;
        else counts2.emplace(END_OF_STRING, count);
      }
    }
    return std::make_pair(counts1, counts2);
  }

private:

  constexpr static char END_OF_STRING = static_cast<char>(0x80);

};

#endif //CAPS_FSA_MIXED_HUFFMAN_ENCODER_H
