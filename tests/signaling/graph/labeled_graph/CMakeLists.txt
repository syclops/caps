# CAPS Graph Unit Test Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_executable(labeled_graph_test labeled_graph_test.cc)
target_link_libraries(labeled_graph_test
        PRIVATE node graph gtest gtest_main
)
gtest_discover_tests(labeled_graph_test)

