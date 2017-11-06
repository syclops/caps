/**
 * Iterable
 */

#ifndef CAPS_ITERABLE_H
#define CAPS_ITERABLE_H

template <typename ReverseIterableType>
class ReverseIterable
{
 public:
  ReverseIterable(const ReverseIterableType& iterable)
    : iterable_{iterable}
  {
    // Nothing to do here.
  }

  typename ReverseIterableType::const_reverse_iterator begin() const
  {
    return iterable_.crbegin();
  }

  typename ReverseIterableType::const_reverse_iterator end() const
  {
    return iterable_.crend();
  }

 private:

  const ReverseIterableType& iterable_;
};

#endif //CAPS_ITERABLE_H
