//
// Created by smaptas on 09/24/18.
//

#ifndef CAPS_BITVECTOR_H
#define CAPS_BITVECTOR_H

// Include C standard libraries.
#include <cmath>
#include <cstddef>

// Include C++ standard libraries.
#include <iostream>
#include <vector>

// Include other headers from this project.
#include "../defs.h"

// Include headers from other projects.

template <typename BitContainerType>
using AllocatorType = typename BitContainerType::allocator_type;

template <typename BitContainerType>
using RefType = typename BitContainerType::reference;

/**
 * Wrapper interface for a container of bits.
 *
 * @tparam BitContainerType - a container-ish type for holding bools.
 */
template <typename BitContainerType = std::vector<bool>>
class BitVector
{

 public:

  BitVector()
    : bits_{}
  {
    // Nothing to do here.
  }

  explicit BitVector(size_t n, bool fill_value = false)
    : bits_(n, fill_value)
  {
    // Nothing to do here.
  }

  explicit BitVector(BitContainerType bits)
    : bits_{bits}
  {
    // Nothing to do here.
  }

  BitVector(const BitVector& bv) = default;

  BitVector(BitVector&& bv) = default;

  BitVector& operator=(const BitVector& bv) = default;

  BitVector& operator=(BitVector&& bv) = default;

  ~BitVector() = default;

  void swap(BitVector& other)
  {
    bits_.swap(other.bits_);
  }

  AllocatorType<BitContainerType> get_allocator() const
  {
    return bits_.get_allocator();
  }

  size_t size() const noexcept
  {
    return bits_.size();
  }

  size_t max_size() const noexcept
  {
    return bits_.max_size();
  }

  size_t capacity() const noexcept
  {
    return bits_.capacity();
  }

  bool empty() const noexcept
  {
    return bits_.empty();
  }

  void reserve(size_t n)
  {
    bits_.reserve(n);
  }

  void shrink_to_fit()
  {
    bits_.shrink_to_fit();
  }

  RefType<BitContainerType> operator[](size_t n)
  {
    return bits_[n];
  }

  bool operator[](size_t n) const
  {
    return bits_[n];
  }

  void resize(size_t n, bool value = false)
  {
    bits_.resize(n, value);
  }

  void push_back(bool b)
  {
    bits_.push_back(b);
  }

  void push_back(const BitVector& bs, bool reserve_space = false)
  {
    if (reserve_space) {
      reserve(size() + bs.size());
    }
    for (size_t i = 0; i < bs.size(); ++i) {
      push_back(static_cast<bool>(bs[i]));
    }
  }

  void push_back(BitVector&& bs, bool reserve_space = false)
  {
    if (reserve_space) {
      reserve(size() + bs.size());
    }
    for (size_t i = 0; i < bs.size(); ++i) {
      push_back(static_cast<bool>(bs[i]));
    }
  }

  void pop_back()
  {
    bits_.pop_back();
  }

  void clear()
  {
    bits_.clear();
  }

  BitVector& flip()
  {
    return bits_.flip();
  }

  void pad(bool b = false)
  {
    resize(BITS_IN_CHAR * static_cast<size_t>(ceil(static_cast<double>(
      size()) / BITS_IN_CHAR)), b);
  }

 private:

  BitContainerType bits_;
};

template <typename BitContainerType>
bool operator==(const BitVector<BitContainerType>& lhs,
                const BitVector<BitContainerType>& rhs)
{
  if (lhs.size() != rhs.size()) {
    return false;
  }
  // Because the underlying container types may have non-standard iterators
  // or no iterators at all, compare by iterating over the indices.
  for (size_t i = 0; i < lhs.size(); ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

template <typename BitContainerType>
bool operator!=(const BitVector<BitContainerType>& lhs,
                const BitVector<BitContainerType>& rhs)
{
  return !(lhs == rhs);
}

// TODO: if necessary, operators <, >, <=, >=

#endif //CAPS_BITVECTOR_H
