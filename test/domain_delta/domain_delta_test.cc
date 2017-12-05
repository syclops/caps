//
// Created by smaptas on 05.12.17.
//

#include "../../tools/cpp/domain_delta/domain_map.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include "gtest/gtest.h"

TEST(DiffTrait, Defaults)
{
  namespace bg = boost::gregorian;
  ASSERT_EQ(diff_trait<std::string>::type_default, "");
  ASSERT_EQ(diff_trait<int>::type_default, 0);
  ASSERT_EQ(diff_trait<float>::type_default, 0.0);
  ASSERT_EQ(diff_trait<bg::date>::type_default, bg::date{});
}