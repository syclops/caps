/**
 * Partial Huffman Coder, excluding strings that appear only once
 * Author: Yucheng Dai <yuchengd@andrew.cmu.edu>
 */

#ifndef CAPS_PARTIAL_HUFFMAN_H
#define CAPS_PARTIAL_HUFFMAN_H

// Include C standard libraries. 
#include <cstdio>

// Include C++ standard libraries. 
#include <optional>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

// Include other headers from this project.
#include "coder.h"
#include "delta_coder.h"
#include "string_coder.h"
#include "huffman_coder.h"

// Include headers from other projects.
template <typename SymbolType, typename EncodingType>
class PartialHuffmanCoder: public HuffmanCoder<SymbolType, EncodingType>
{
public:

	template<typename MapType>
	explicit PartialHuffmanCoder(const MapType& counts)
		: encoding_map_{}, decoding_symbols_{}, decoding_indices_{},
			decoding_codes_{}, max_length_{0}, string_coder_{}
	{
		create_codebook_generic(counts);
	}


protected:

	template <typename MapType>
	std::shared_ptr<HuffmanNode> make_tree(const MapType& counts)
	{
		using PtrType = std::shared_ptr<HuffmanNode>;
		std::priority_queue<PtrType, std::vector<PtrType>, HuffmanNodeComp> heap;
		for (auto it: counts) {
			const auto& [symbol, count] = it;
			if (count == 1){
				heap.push(std::make_shared<HuffmanNode>(symbol, count));
			}
		}
		while (heap.size() > 1) {
			auto first = heap.top();
			heap.pop();
			auto second = heap.top();
			heap.pop();
			heap.push(std::make_shared<HuffmanNode>(first, second));
		}
		return heap.top();
	}

	void encode_impl(const SymbolType& value, EncodingType* encoding) const override
	{
		if (encoding_map_.find(value)!=encoding_map_.end()){
			encoding->push_back(true);
			encoding->push_back(encoding_map_.at(value));
		}else{
			encoding->push_back(false);
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

	StringCoder<EncodingType> string_coder_;


private:

	auto inc_size(std::optional<std::pair<SymbolType, size_t>> decode_opt)
	{
		if (decode_opt==std::nullopt) return decode_opt;
		else{
			decode_opt->second++;
			return decode_opt;
		}
	}
};


#endif