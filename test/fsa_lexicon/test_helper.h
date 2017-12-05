//
// Created by smaptas on 13.11.17.
//

#ifndef CAPS_TEST_HELPER_H
#define CAPS_TEST_HELPER_H

#include <set>
#include <sstream>
#include <string>

inline std::stringstream set_stream(std::set<std::string> strings)
{
  std::stringstream stream;
  for (auto str: strings) {
    stream << str << std::endl;
  }
  return stream;
}

#endif //CAPS_TEST_HELPER_H
