# CAPS Graph Connected Component Unit Test Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_executable(connected_component_test connected_component_test.cc)
target_link_libraries(connected_component_test
        PUBLIC node
        PRIVATE graph connected_component gtest gtest_main
)
gtest_discover_tests(connected_component_test)

