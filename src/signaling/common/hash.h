/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_HASH_H
#define CAPS_HASH_H

// Include C standard libraries.
#include <cstddef>

// Include C++ standard libraries.

// Include other headers from this project.

// Include headers from other projects.

template <typename T>
struct ConstPointerHash
{
  size_t operator()(const T* value) const
  {
    return std::hash<T*>{}(const_cast<T*>(value));
  }
};

template <typename T>
struct ConstRefHash
{
  size_t operator()(const T& value) const
  {
    return std::hash<T&>{}(const_cast<T&>(value));
  }
};

#endif //CAPS_HASH_H
