# Signaling Common Utilities Unit Test Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

include(GoogleTest)

# TODO: contains

# TODO: compare

# TODO: io_option

# Time measurement
# NOTE: this probably isn't necessary.
#add_executable(measure_time_test measure_time_test.cc)
#target_link_libraries(measure_time_test
#    PRIVATE measure_time gtest gtest_main
#)
#gtest_discover_tests(measure_time_test)

# Power set
add_executable(powerset_test powerset_test.cc)
target_link_libraries(powerset_test
    PRIVATE powerset gtest gtest_main
)
gtest_discover_tests(powerset_test)

# TODO: reverse_iterable
