# CAPS Node Unit Test Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_executable(node_test node_test.cc)
target_link_libraries(node_test
        PUBLIC node
        PRIVATE contains reverse_iterable gtest gtest_main
)
gtest_discover_tests(node_test)

