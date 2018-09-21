//
// Created by smaptas on 27.10.17.
//

#include <iostream>
#include <set>

#include "../../../src/signaling/common/powerset.h"

#include "gtest/gtest.h"

TEST(PowersetTest, SetTest)
{
  std::set<int> set = {1, 2, 3};
  auto pset = powerset(set);
  ASSERT_EQ(8, pset.size());
  // TODO: check that every expected subset is in the set.
}

// TODO: generalize this test to take a set of generic type as a parameter
//       find a way to generalize checking that each subset is in the set.
