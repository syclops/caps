//
// Created by smaptas on 09/14/18.
//

#ifndef CAPS_COMPARE_H
#define CAPS_COMPARE_H

#include <utility>

template<typename T1, typename T2>
struct first_less {
  bool operator()(const std::pair<T1, T2>& lhs,
                  const std::pair<T1, T2>& rhs) const {
    return lhs.first < rhs.first;
  }
};

template<typename T1, typename T2>
struct first_greater {
  bool operator()(const std::pair<T1, T2>& lhs,
                  const std::pair<T1, T2>& rhs) const {
    return lhs.first > rhs.first;
  }
};

template<typename T1, typename T2>
struct second_less {
  bool operator()(const std::pair<T1, T2>& lhs,
                  const std::pair<T1, T2>& rhs) const {
    return lhs.second < rhs.second;
  }
};

template<typename T1, typename T2>
struct second_greater {
  bool operator()(const std::pair<T1, T2>& lhs,
                  const std::pair<T1, T2>& rhs) const {
    return lhs.second > rhs.second;
  }
};

#endif //CAPS_COMPARE_H
