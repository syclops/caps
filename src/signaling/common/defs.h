/**
 * One-sentence description of file contents.
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_DEFS_H
#define CAPS_DEFS_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <limits>

// Include other headers from this project.

// Include headers from other projects.

using char_limits = std::numeric_limits<char>;

constexpr size_t BITS_IN_CHAR = static_cast<size_t>(char_limits::digits)
                                + static_cast<size_t>(char_limits::is_signed);

#endif //CAPS_DEFS_H
