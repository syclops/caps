//
// Created by smaptas on 27.10.17.
//

#include <iostream>
#include <set>

#include "../../src/cpp/common/powerset.h"

#include "gtest/gtest.h"

TEST(PowersetTest, SetTest)
{
  std::set<int> set = {1, 2, 3};
  for (auto subset: powerset(set)) {
    std::cerr << "{";
    for (auto val: subset) {
      std::cerr << val;
    }
    std::cerr << "}" << std::endl;
  }
}
