# CAPS Encoding Unit Test Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

# Huffman encoding
add_executable(huffman_test huffman_test.cc)
target_link_libraries(huffman_test
    PRIVATE contains huffman gtest gtest_main
)
gtest_discover_tests(huffman_test)