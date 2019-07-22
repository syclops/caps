/**
 * Mixed Huffman Coder
 * Author: Yucheng Dai <yuchengd@andrew.cmu.edu>
 */

#ifndef CAPS_MIXED_HUFFMAN_H
#define CAPS_MIXED_HUFFMAN_H

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
#include "char_coder.h"
#include "char_huffman_coder.h"
#include "huffman_coder.h"

// Include headers from other projects.
template <typename SymbolType, typename EncodingType>
class MixedHuffmanCoder: public HuffmanCoder<SymbolType, EncodingType>
{
public:

	template<typename MapType>
	explicit MixedHuffmanCoder(const MapType& counts1, std::unordered_map<char, size_t> counts2)
		: HuffmanCoder<SymbolType, EncodingType>(counts1), 
		  char_huffman_coder_{counts2}
	{
		// Nothing to do here. 
	}

	inline bool valid_value(const SymbolType& value) const override
	{
		return (HuffmanCoder<SymbolType, EncodingType>::valid_value(value)) || (char_huffman_coder_.valid_value(value));
	}

	inline size_t value_size(const SymbolType& value) const override
	{
		if ((HuffmanCoder<SymbolType, EncodingType>::encoding_map_.find(value))
			!= (HuffmanCoder<SymbolType, EncodingType>::encoding_map_.end()))
			return 1+HuffmanCoder<SymbolType, EncodingType>::encoding_map_.at(value).size();
		else
			return 1+char_huffman_coder_.value_size(value);
	}

	EncodingType get_codebook(
		std::shared_ptr<Coder<SymbolType, EncodingType>> symbol_coder) const
	{
		EncodingType buffer;
		buffer.push_back(HuffmanCoder<SymbolType, EncodingType>::get_codebook(symbol_coder));
		auto char_label_coder = std::make_shared<CharCoder<>>();
		buffer.push_back(char_huffman_coder_.get_codebook(
			std::static_pointer_cast<Coder<char, EncodingType>>(char_label_coder)));
		return buffer;
	}

protected:

	void encode_impl(const SymbolType& value, EncodingType* encoding) const override
	{
		if ((HuffmanCoder<SymbolType, EncodingType>::encoding_map_.find(value))
			!=(HuffmanCoder<SymbolType, EncodingType>::encoding_map_.end())){
			encoding->push_back(true);
			encoding->push_back(HuffmanCoder<SymbolType, EncodingType>::encoding_map_.at(value));
		}else{
			encoding->push_back(false);
			char_huffman_coder_.encode(value, encoding);
		}
	}

	std::optional<std::pair<SymbolType, size_t>> decode_impl(
		const EncodingType& buffer, size_t position) const override
	{
		if (buffer[position] == true){
			return inc_size(HuffmanCoder<SymbolType, EncodingType>::decode_impl(buffer, position+1));
		}else{
			return inc_size(char_huffman_coder_.decode(buffer, position+1));
		}
	}

	std::optional<std::pair<SymbolType, size_t>> inc_size
			(const std::optional<std::pair<SymbolType, size_t>> decode_opt) const
	{
		if (!decode_opt.has_value()) return decode_opt;
		else{
			return std::make_pair(decode_opt->first, decode_opt->second+1);
		}
	}

	CharHuffmanCoder<SymbolType, EncodingType> char_huffman_coder_;
	constexpr static char END_OF_STRING = static_cast<char>(0x80);
};


#endif //CAPS_MIXED_HUFFMAN_H