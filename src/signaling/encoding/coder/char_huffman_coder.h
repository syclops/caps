/**
 * Char Huffman Coder, excluding strings that appear only once
 * Author: Yucheng Dai <yuchengd@andrew.cmu.edu>
 */

#ifndef CAPS_CHAR_HUFFMAN_H
#define CAPS_CHAR_HUFFMAN_H

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
class CharHuffmanCoder: public HuffmanCoder<char, EncodingType>
{
public:

	template<typename MapType>
	explicit CharHuffmanCoder(const MapType& counts)
		: HuffmanCoder<char, EncodingType>(counts)
	{
		// Nothing to do here. 
	}

	inline bool valid_value(const SymbolType& value) const override
	{
		for(const auto& ch: value){
			if (!HuffmanCoder<char, EncodingType>::valid_value(ch)) return false;
		}
		return true;
	}

	inline size_t value_size(const SymbolType& value) const override
	{
		size_t ans = 0;
		for(const auto& ch: value){
			ans += HuffmanCoder<char, EncodingType>::value_size(ch);
		}
		ans += HuffmanCoder<char, EncodingType>::value_size(END_OF_STRING);
		return ans;
	}

protected:

	void encode_impl(const SymbolType& value, EncodingType* encoding) const override
	{
		for(const auto& ch: value){
			encoding->push_back(HuffmanCoder<char, EncodingType>::encoding_map_.at(ch));
		}
		encoding->push_back(HuffmanCoder<char, EncodingType>::encoding_map_.at(END_OF_STRING));
	}

	std::optional<std::pair<SymbolType, size_t>> decode_impl(
		const EncodingType& buffer, size_t position) const override
	{
		SymbolType value{};
		for (size_t i = position; i < buffer.size();) {
			auto decoded = HuffmanCoder<char, EncodingType>::decode(buffer, i);
			if (!decoded.has_value()) {
				return std::nullopt;
			}
			auto decoded_char = decoded->first;
			i += decoded->second;
			if (decoded_char==END_OF_STRING){
				return std::make_optional(std::make_pair(value, i-position));
			}
			value.push_back(decoded_char);
		}
		return std::nullopt;
	}

private:

	constexpr static char END_OF_STRING = static_cast<char>(0x80);
};


#endif //CAPS_CHAR_HUFFMAN_CODER_H