/**
 * Implementation of a small int encoder/decoder extending Coder
 * Author: Yucheng Dai <yuchengd@andrew.cmu.edu>
 * 
 */

#ifndef CAPS_SMALL_INT_CODER_H
#define CAPS_SMALL_INT_CODER_H

//Include C Standard Libraries

//Include C++ Standard Libraries
#include <optional>
#include <utility>

// Include other headers from this project.
#include "char_coder.h"
#include "coder.h"
#include "../defs.h"
#include "../bitvector/bitvector.h"

// Include headers from other projects.
template <typename StringType = std::string, 
		  typename EncodingType = BitVector<>>
class SmallIntCoder: public Coder<StringType, EncodingType>
{
public:

	SmallIntCoder(): 
		Coder<StringType, EncodingType>{}
	{
//		for(int i=0;i<256;i++) encode_table[i] = 0;
//		for(int i=0;i<256;i++) decode_table[i] = 0;
//		small_int_num = 0;
		create_small_int_table();
	}

	inline bool valid_value(const StringType& value) const override
	{
		return !value.empty();
	}

	inline size_t value_size(const StringType& value) const override
	{
		return ENCODING_BITS * (value.size() + 1);
	}

protected:

	void encode_impl(const StringType& value, EncodingType* buffer) const override {
		for(auto itr = value.begin(); itr != value.end(); ++itr){
			auto c = ctosi(*itr);
			for(size_t i = 0; i < ENCODING_BITS; i++){
				buffer->push_back((bool)(c&1));
				c>>=1;
			}
		}
//		char_coder_.encode(static_cast<char>(tmp), buffer);
		for(size_t i=0;i<ENCODING_BITS;i++) buffer->push_back(true);
	}

	std::optional<std::pair<StringType, size_t>> 
		decode_impl(const EncodingType& buffer, const size_t position) const override {
		std::string value;
		for(size_t i = position; i < buffer.size(); i+= ENCODING_BITS){
			auto si = 0;
			size_t j;
			for(j = i; j < buffer.size() && j - i < ENCODING_BITS; j++){
				si = (si << 1) + buffer[j];
			}
			if (j==buffer.size() && j - i < ENCODING_BITS){
				return std::nullopt;
			}

			if (si==END_OF_STRING){
				return std::make_optional(std::make_pair(value, value_size(value)));
			}
			auto decoded_char = sitoc(si);
			if (decoded_char==0){
				return std::nullopt;
			}
			value.push_back(decoded_char);
		}
		return std::nullopt;
	}

	void create_small_int_table(){
		int table_list[43] = {42,45,46,47,48,49,50,51,52,53,54,55,56,
			57,58,64,95,97,98,99,100,101,102,103,104,105,106,107,108,109,
			110,111,112,113,114,115,116,117,118,119,120,121,122};
		int small_int_num=0;
		for(auto ch: table_list){
			encode_table[ch]=++small_int_num;
			decode_table[small_int_num]=ch;
		}
	}

private:
	const size_t ENCODING_BITS = 6;
//	const CharCoder<EncodingType> char_coder_;

	unsigned char encode_table[256]; //char -> small int
	unsigned char decode_table[256]; //small int -> char
//	int small_int_num; //current small int reached
	constexpr static unsigned char END_OF_STRING = static_cast<char>(0x3F);
//	constexpr static unsigned char SMALL_INT_MASK = static_cast<char>(0x3F);

	unsigned char ctosi(const unsigned char x) const{
//		if (encode_table[x]!=0) return encode_table[x];
//		encode_table[x]=++small_int_num;
//		decode_table[small_int_num]=x;
		return encode_table[x];
	}

	unsigned char sitoc(const unsigned char x) const{
		return decode_table[x];
	}

};

#endif //CAPS_SMALL_INT_CODER_H
