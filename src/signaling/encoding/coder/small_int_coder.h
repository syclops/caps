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
		  typename EncodingType>
class SmallIntCoder: public Coder<StringType, EncodingType>
{
public:

	SmallIntCoder(): 
		Coder<StringType, EncodingType>{}, char_coder_{}
	{
//		for(int i=0;i<256;i++) encode_table[i] = 0;
//		for(int i=0;i<256;i++) decode_table[i] = 0;
//		small_int_num = 0;
		create_small_int_table();
		tmp = 0;
		pos = 0;
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
		tmp = 0;
		pos = 0;
		for(auto itr = value.begin(); itr != value.end(); ++itr){
			unsigned char c = ctosi(*itr);
			tmp |= c << pos;
			if (pos + ENCODING_BITS >= BITS_IN_CHAR){
				pos = pos + ENCODING_BITS - BITS_IN_CHAR;
				char_coder_.encode(static_cast<char>(tmp), buffer);
				tmp = c >> (BITS_IN_CHAR - pos);
			}else{
				pos = pos + ENCODING_BITS;
			}
		}
		tmp |= END_OF_STRING << pos;
		if (pos + ENCODING_BITS >= BITS_IN_CHAR){
			pos = pos + ENCODING_BITS - BITS_IN_CHAR;
			char_coder_.encode(static_cast<char>(tmp), buffer);
			tmp = END_OF_STRING >> (BITS_IN_CHAR - pos);
		}else{
			pos = pos + ENCODING_BITS;
		}
//		char_coder_.encode(static_cast<char>(tmp), buffer);
		for(int i=0;i<ENCODING_BITS;i++) buffer->push_back(true);
	}

	std::optional<std::pair<StringType, size_t>> 
		decode_impl(const EncodingType& buffer, const size_t position) const override {
		std::string value;
		for(size_t i = position; i < buffer.size(); i+= ENCODING_BITS){
			auto decoded = char_coder_.decode(buffer, i);
			if (!decoded.has_value()){
				return std::nullopt;
			}

			unsigned char si = (decoded->first) & SMALL_INT_MASK;
			if (si==END_OF_STRING){
				return std::make_optional(std::make_pair(value, value_size(value)));
			}
			unsigned char decoded_char = sitoc(si);
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

	unsigned char tmp;
	size_t pos;

private:
	const size_t ENCODING_BITS = 6;
	const CharCoder<EncodingType> char_coder_;

	unsigned char encode_table[256]; //char -> small int
	unsigned char decode_table[256]; //small int -> char
//	int small_int_num; //current small int reached
	constexpr static unsigned char END_OF_STRING = static_cast<char>(0x3F);
	constexpr static unsigned char SMALL_INT_MASK = static_cast<char>(0x3F);

	unsigned char ctosi(unsigned char x){
//		if (encode_table[x]!=0) return encode_table[x];
//		encode_table[x]=++small_int_num;
//		decode_table[small_int_num]=x;
		return encode_table[x];
	}

	unsigned char sitoc(unsigned char x){
		return decode_table[x];
	}

};

#endif //CAPS_SMALL_INT_CODER_H
