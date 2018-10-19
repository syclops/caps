/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_SIGNED_INT_CODER_H
#define CAPS_SIGNED_INT_CODER_H

// Include C standard libraries.

// Include C++ standard libraries.

// Include other headers from this project.
#include "coder.h"

// Include headers from other projects.

template <typename SignedIntType, typename EncodingType>
class SignedIntCoder: public Coder<SignedIntType, EncodingType>
{
 public:

  SignedIntCoder() = delete;

  explicit SignedIntCoder(Coder<SignedIntType, EncodingType>* base_coder)
    : base_coder_{base_coder}
  {
    // Nothing to do here.
  }

  inline bool valid_value(const SignedIntType& value) const override
  {
    return true;
  }

  inline size_t value_size(const SignedIntType& value) const override
  {
    return base_coder_->value_size(value) + 1;
  }

  const Coder<SignedIntType, EncodingType>& get_coder() const
  {
    return *base_coder_;
  }

 protected:

  void encode_impl(const SignedIntType& value,
                   EncodingType* encoding) const override
  {
    auto negative = (value < 0);
    encoding->push_back(negative);
    base_coder_->encode(negative ? -value : value, encoding);
  }

  std::optional<std::pair<SignedIntType, size_t>> decode_impl(
    const EncodingType& buffer, const size_t position) const override
  {
    auto decoded_value = base_coder_->decode(buffer, position + 1);
    if (!decoded_value.has_value()) {
      return std::nullopt;
    }
    auto negative = buffer[position];
    const auto& [value, size] = *decoded_value;
    return std::make_optional(std::make_pair(negative ? -value : value,
                                             size + 1));
  }

 private:

  std::unique_ptr<Coder<SignedIntType, EncodingType>> base_coder_;

};

#endif //CAPS_SIGNED_INT_CODER_H
