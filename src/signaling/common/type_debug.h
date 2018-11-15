/**
 * Debugging header to view full type information.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_TYPE_DEBUG_H
#define CAPS_TYPE_DEBUG_H

// Include C standard libraries.
#include <cstddef>
#include <cstring>

// Include C++ standard libraries.
#include <ostream>
#include <stdexcept>
#include <type_traits>

// Include other headers from this project.

// Include headers from other projects.

template <class T>
constexpr std::string_view type_name()
{
  using namespace std;
#ifdef __clang__
  string_view p = __PRETTY_FUNCTION__;
  return string_view(p.data() + 34, p.size() - 34 - 1);
#elif defined(__GNUC__)
  string_view p = __PRETTY_FUNCTION__;
#  if __cplusplus < 201402
    return string_view(p.data() + 36, p.size() - 36 - 1);
#  else
    return string_view(p.data() + 49, p.find(';', 49) - 49);
#  endif
#elif defined(_MSC_VER)
    string_view p = __FUNCSIG__;
    return string_view(p.data() + 84, p.size() - 84 - 7);
#endif
}

#endif //CAPS_TYPE_DEBUG_H
