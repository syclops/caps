/**
 * Unifying comparator for smart and raw pointers.
 *
 * Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>
 */

#ifndef CAPS_POINTER_COMPARATOR_H
#define CAPS_POINTER_COMPARATOR_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <iostream>
#include <memory>

// Include other headers from this project.

// Include headers from other projects.

template <typename T>
struct pointer_comparator
{
  // Define is_transparent to allow the comparator to accept heterogeneous
  // pointer types.
  using is_transparent = void;

  /**
   * Helper struct to convert smart pointer types into raw pointers for
   * comparison purposes.
   */
  struct helper
  {
    helper()
      : pointer{nullptr}
    {
      // Nothing to do here.
    }

    helper(const helper&) = default;

    helper(T* p)
      : pointer{p}
    {
      // Nothing to do here.
    }

    helper(const std::weak_ptr<T>& wp)
      : pointer{wp.lock().get()}
    {
      // Nothing to do here.
    }

    helper(const std::shared_ptr<T>& sp)
      : pointer{sp.get()}
    {
      // Nothing to do here.
    }

    template <typename Deleter = std::default_delete<T>>
    helper(const std::unique_ptr<T, Deleter>& up)
      : pointer{up.get()}
    {
      // Nothing to do here.
    }

    bool operator<(helper o) const
    {
      return std::less<T*>()(pointer, o.pointer);
    }

    T* pointer;
  };

  bool operator()(const helper&& lhs, const helper&& rhs) const
  {
    return lhs < rhs;
  }
};

#endif //CAPS_POINTER_COMPARATOR_H
