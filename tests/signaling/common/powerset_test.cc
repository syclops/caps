//
// Created by smaptas on 27.10.17.
//

#include <iostream>
#include <set>

#include "../../../src/signaling/common/powerset.h"

#include "gtest/gtest.h"

TEST(PowersetTest, SetSize)
{
  std::set<int> set = {1, 2, 3};
  auto pset = powerset(set);
  ASSERT_EQ(8, pset.size());
    std::set<int> set2 = {1, 2, 3, 4};
    auto pset2 = powerset(set2);
    ASSERT_EQ(16, pset2.size());
}
TEST(PowersetTest, SetContains)
{
    std::set<int> set = {1, 2, 3};
    auto pset = powerset(set);
    std::set<int> set1 = {};
    std::set<int> set2 = {1};
    std::set<int> set3 = {2};
    std::set<int> set4 = {3};
    std::set<int> set5 = {1, 2};
    std::set<int> set6 = {1, 3};
    std::set<int> set7 = {2, 3};
    std::set<int> set8 = {1, 2, 3};
    std::set<std::set<int>> pset2 = {set1, set2, set3, set4, set5, set6, set7, set8};


ASSERT_EQ(pset2, pset);
}


// TODO: generalize this test to take a set of generic type as a parameter
//       find a way to generalize checking that each subset is in the set.
