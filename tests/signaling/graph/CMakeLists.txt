# CAPS Graph Unit Test Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

include(GoogleTest)

# Node
add_executable(node_test node_test.cc)
target_link_libraries(node_test
    PUBLIC node
    PRIVATE contains reverse_iterable gtest gtest_main
)
gtest_discover_tests(node_test)

# Labeled Graph
add_executable(labeled_graph_test labeled_graph_test.cc)
target_link_libraries(labeled_graph_test
  PRIVATE node graph gtest gtest_main
)
gtest_discover_tests(labeled_graph_test)

# TODO: visitor

# TODO: graph_search

# Connected Component
add_executable(connected_component_test connected_component_test.cc)
target_link_libraries(connected_component_test
    PUBLIC node
    PRIVATE graph connected_component gtest gtest_main
)
gtest_discover_tests(connected_component_test)

# TODO: ordering

