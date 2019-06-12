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
#include "fsa_encoder.h"

template <typename BitVectorType>
class FSACharEncoder: public FSAEncoder<BitVectorType>
{
public:
	FSACharEncoder() = delete;

	explicit FSAHuffmanEncoder(const FSALexicon& lexicon)
		: FSAEncoder<BitVectorType>{lexicon}
	{
		auto label_coder = std::make_shared<SmallIntCoder<std::string, BitVectorType>>();
		using LabelCoderType = Coder<std::string, BitVectorType>;
		FSAEncoder<BitVectorType>::label_coder_ =
			std::static_pointer_cast<LabelCoderType>(label_coder);
		FSAEncoder<BitVectorType>::order_nodes();

		auto destination_coder = std::make_shared<SmallIntCoder<int, BitVectorType>>();
		using DestCoderType = Coder<int, BitVectorType>;
		FSAEncoder<BitVectorType>::destination_coder_ =
			std::static_pointer_cast<DestCoderType>(destination_coder);
	}
};

#endif