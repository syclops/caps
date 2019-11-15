/**
 * Partial Huffman Coder, excluding strings that appear only once
 * Author: Yucheng Dai <yuchengd@andrew.cmu.edu>
 */

#ifndef CAPS_PARTIAL_HUFFMAN_H
#define CAPS_PARTIAL_HUFFMAN_H

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
#include "small_int_coder.h"
#include "huffman_coder.h"

// Include headers from other projects.
template <typename SymbolType, typename EncodingType>
class PartialHuffmanCoder: public HuffmanCoder<SymbolType, EncodingType>
{
public:

	template<typename MapType>
	explicit PartialHuffmanCoder(const MapType& counts)
		: HuffmanCoder<SymbolType, EncodingType>(counts), 
		  small_int_coder_{}
	{
		// Nothing to do here. 
	}

	inline bool valid_value(const SymbolType& value) const override
	{
		return (HuffmanCoder<SymbolType, EncodingType>::valid_value(value)) || (small_int_coder_.valid_value(value));
	}

	inline size_t value_size(const SymbolType& value) const override
	{
		if ((HuffmanCoder<SymbolType, EncodingType>::encoding_map_.find(value))
			!= (HuffmanCoder<SymbolType, EncodingType>::encoding_map_.end()))
			return 1+HuffmanCoder<SymbolType, EncodingType>::encoding_map_.at(value).size();
		else
			return 1+small_int_coder_.value_size(value);
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
			small_int_coder_.encode(value, encoding);
		}
	}

	std::optional<std::pair<SymbolType, size_t>> decode_impl(
		const EncodingType& buffer, size_t position) const override
	{
		if (buffer[position] == true){
			return inc_size(HuffmanCoder<SymbolType, EncodingType>::decode_impl(buffer, position+1));
		}else{
			return inc_size(small_int_coder_.decode(buffer, position+1));
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

	SmallIntCoder<> small_int_coder_;
};


#endif //CAPS_PARTIAL_HUFFMAN_H