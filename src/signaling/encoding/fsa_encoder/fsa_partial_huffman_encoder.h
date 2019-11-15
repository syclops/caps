/**
 * FSA Partial Huffman Encoder
 * Author: Yucheng Dai <yuchengd@andrew.cmu.edu>
 */

#ifndef CAPS_FSA_PARTIAL_HUFFMAN_ENCODER_H
#define CAPS_FSA_PARTIAL_HUFFMAN_ENCODER_H

// Include C++ standard libraries.
#include <memory>
#include <string>

// Include other headers from this project.
#include "../../lexicon/fsa_lexicon/fsa_lexicon.h"
#include "../coder/partial_huffman_coder.h"
#include "../coder/huffman_coder.h"
#include "../coder/signed_int_coder.h"
#include "fsa_encoder.h"

template <typename BitVectorType>
class FSAPartialHuffmanEncoder: public FSAEncoder<BitVectorType>
{
 public:

  FSAPartialHuffmanEncoder() = delete;

  explicit FSAPartialHuffmanEncoder(const FSALexicon& lexicon)
    : FSAEncoder<BitVectorType>{lexicon}
  {
    FSAEncoder<BitVectorType>::order_nodes();
    auto diff_counts = get_ordering_diff_counts(lexicon);
    auto destination_coder = std::make_shared<HuffmanCoder<int, BitVectorType>>(
      diff_counts);
    using DestCoderType = Coder<int, BitVectorType>;
    FSAEncoder<BitVectorType>::destination_coder_ =
      std::static_pointer_cast<DestCoderType>(destination_coder);

    auto counts = get_new_counts(get_label_counts(lexicon));
    auto label_coder = std::make_shared<PartialHuffmanCoder<std::string,
                                                     BitVectorType>>(
      counts);
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
    using LabelHuffmanType = PartialHuffmanCoder<LabelType, BitVectorType>;
    using DestType = int;
    using DestAbstractType = Coder<DestType, BitVectorType>;
    using DestCoderType = SignedIntCoder<DestType, BitVectorType>;
    using DestBaseCoderType = DeltaCoder<DestType, BitVectorType>;
    using DestHuffmanType = HuffmanCoder<DestType, BitVectorType>;

    auto string_coder = std::make_shared<LabelCoderType>();
    auto label_codebook = std::static_pointer_cast<LabelHuffmanType>(
        FSAEncoder<BitVectorType>::label_coder_)->get_codebook(
          std::static_pointer_cast<LabelAbstractType>(string_coder));
//    std::cerr << label_codebook.size() << std::endl;
    buffer.push_back(label_codebook);

    auto dest_coder = std::make_shared<DestCoderType>(new DestBaseCoderType);
    auto dest_codebook = std::static_pointer_cast<DestHuffmanType>(
        FSAEncoder<BitVectorType>::destination_coder_)->get_codebook(
          std::static_pointer_cast<DestAbstractType>(dest_coder));
//    std::cerr << dest_codebook.size() << std::endl;
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

  LabeledGraph::LabelMap get_new_counts(const LabeledGraph::LabelMap& counts) const{
    LabeledGraph::LabelMap counts2;
    for (const auto& [symbol, count]: counts){
      if (symbol.length()>=30) continue;
      if (count>3) counts2.emplace(symbol, count);
    }
    return counts2;
  }
};

#endif //CAPS_FSA_PARTIAL_HUFFMAN_ENCODER_H
