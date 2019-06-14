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
#include "string_coder.h"
#include "huffman_coder.h"

// Include headers from other projects.
template <typename SymbolType, typename EncodingType>
class PartialHuffmanCoder: public HuffmanCoder<SymbolType, EncodingType>
{
public:

	template<typename MapType>
	explicit PartialHuffmanCoder(const MapType& counts)
		: HuffmanCoder<SymbolType, EncodingType>(modify_counts<MapType>(counts)), 
		  string_coder_{}
	{
		// Nothing to do here. 
	}


protected:

	void encode_impl(const SymbolType& value, EncodingType* encoding) const override
	{
		if (HuffmanCoder<SymbolType, EncodingType>::encoding_map_.find(value)
			!=HuffmanCoder<SymbolType, EncodingType>::encoding_map_.end()){
			encoding->push_back(static_cast<EncodingType>(true));
			encoding->push_back(HuffmanCoder<SymbolType, EncodingType>::encoding_map_.at(value));
		}else{
			encoding->push_back(static_cast<EncodingType>(false));
			encoding->push_back(string_coder_.encode(value));
		}
	}

	std::optional<std::pair<SymbolType, size_t>> decode_impl(
		const EncodingType& buffer, size_t position) const override
	{
		if (buffer[position] == true){
			return inc_size(HuffmanCoder<SymbolType, EncodingType>::decode_impl(buffer, position+1));
		}else{
			return inc_size(string_coder_.decode(buffer, position+1));
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

	StringCoder<> string_coder_;


private:
	template <typename MapType>
	const MapType& modify_counts(const MapType& counts){
		static MapType counts2{};
		for (auto it: counts) {
			const auto& [symbol, count] = it;
			if (count != 1){
				counts2.insert(std::make_pair(symbol, count));
			}
		}
		return static_cast<const MapType>(counts2);
	}
};


#endif