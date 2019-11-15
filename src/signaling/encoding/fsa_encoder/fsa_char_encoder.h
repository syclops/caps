/**
 * Author: Yucheng Dai <yuchengd@andrew.cmu.edu>
 */

#ifndef CAPS_FSA_CHAR_ENCODER_H
#define CAPS_FSA_CHAR_ENCODER_H

// Include C++ standard libraries.
#include <functional>
#include <memory>
#include <string>

// Include other headers from this project.
#include "../../lexicon/fsa_lexicon/fsa_lexicon.h"
#include "../coder/small_int_coder.h"
#include "../coder/huffman_coder.h"
#include "fsa_encoder.h"

template <typename BitVectorType>
class FSACharEncoder: public FSAEncoder<BitVectorType>
{
public:
	FSACharEncoder() = delete;

	explicit FSACharEncoder(const FSALexicon& lexicon)
		: FSAEncoder<BitVectorType>{lexicon}
	{
		auto label_coder = std::make_shared<SmallIntCoder<std::string, BitVectorType>>();
		using LabelCoderType = Coder<std::string, BitVectorType>;
		FSAEncoder<BitVectorType>::label_coder_ =
			std::static_pointer_cast<LabelCoderType>(label_coder);
/*		FSAEncoder<BitVectorType>::order_nodes();

		auto diff_counts = get_ordering_diff_counts(lexicon);
		auto destination_coder = std::make_shared<HuffmanCoder<int, BitVectorType>>(
		  diff_counts);
		using DestCoderType = Coder<int, BitVectorType>;
		FSAEncoder<BitVectorType>::destination_coder_ =
			std::static_pointer_cast<DestCoderType>(destination_coder);
*/	}

/*
protected:

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
  }*/
};

#endif //CAPS_FSA_CHAR_ENCODER_H