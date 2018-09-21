# Google Test Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

if (NOT gtest_POPULATED)
  FetchContent_Populate(gtest)
  add_subdirectory(${gtest_SOURCE_DIR} ${gtest_BINARY_DIR})
  include(GoogleTest)
endif()
