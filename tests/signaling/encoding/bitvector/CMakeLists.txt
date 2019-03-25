# CAPS Bitvector Unit Test Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

find_package(Boost REQUIRED)

# STL vector-based implementation test

add_executable(bitvector_test bitvector_test.cc)
target_link_libraries(bitvector_test
    PUBLIC Boost::boost
    PRIVATE bitvector gtest gtest_main
)
gtest_discover_tests(bitvector_test)