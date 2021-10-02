/**
 * One-sentence description of file contents.
 * Practicing making a simple coder to get used to the codebase
 * Using form where 1 is a number and 0 is a terminator
 * Author: Micah Reid <mreid@olin.edu>
 */

#ifndef CAPS_UNARY_CODER_MICAH_PRACTICE_H
#define CAPS_UNARY_CODER_MICAH_PRACTICE_H




// Include C standard libraries.
#include <cstddef>

// Include C++ standard libraries.
#include <optional>
#include <utility>

// Include other headers from this project.
#include "coder.h"

// Include headers from other projects.

template <typename IntType, typename BitVectorType>
class UnaryCoderMicahPractice: public Coder<IntType, BitVectorType>
{
public:

    UnaryCoderMicahPractice() =  default;

    inline bool valid_value(const IntType& value) const override
    {
        return value >= 0;
    }

    inline size_t value_size(const IntType& value) const override
    {
        return value + 1;
    }

protected:
    void encode_impl(const IntType& value, BitVectorType* encoding) const override
    {
        for (auto i = value; i > 0; --i) {
            encoding->push_back(1);
        }
        encoding->push_back(0);
    }

    std::optional<std::pair<IntType, size_t>> decode_impl(
            const BitVectorType& buffer, const size_t position = 0) const override
    {
        for (size_t i = 0; i < buffer.size(); ++i){
            if(buffer[i] == 0) {
                return std::make_optional(std::make_pair(IntType(i), i + 1));
            }
        }
        return std::nullopt;
    }

};


#endif //CAPS_UNARY_CODER_MICAH_PRACTICE_H


