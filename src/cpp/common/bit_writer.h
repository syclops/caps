//
// Created by smaptas on 29.11.17.
//

#ifndef CAPS_BIT_WRITER_H
#define CAPS_BIT_WRITER_H

#include <bitset>
#include <iostream>

class BitWriter
{
 public:

  explicit BitWriter(std::ostream& stream)
    : buffer_size_(0), buffer_(8), stream_(stream)
  {
    // Nothing to do here.
  }

  void write_bit(bool bit)
  {
    buffer_[buffer_size_] = bit;
    ++buffer_size_;
    if (buffer_size_ % 8 == 0){
      flush_to_stream();
    }
  }

  void flush()
  {
    while (buffer_size_ % 8 != 0) {
      write_bit(false);
    }
  }

  int buffer_size() const
  {
    return buffer_size_;
  }

  bool buffer_bit(int i) const
  {
    return buffer_[i];
  }

 private:

  void flush_to_stream()
  {
    unsigned char c = 0;
    for (int i = 0; i < 8; ++i) {
      c += buffer_[i] << (7 - i);
    }
    stream_ << c;
    buffer_.reset();
    buffer_size_ = 0;
  }

  int buffer_size_;
  std::bitset<8> buffer_;
  std::ostream& stream_;
};

#endif //CAPS_BIT_WRITER_H
