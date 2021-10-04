# CAPS Coder Unit Test Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

# Huffman encoding
#add_executable(huffman_test coder/huffman_test.cc)
#target_link_libraries(huffman_test
#        PRIVATE contains huffman bitvector gtest gtest_main
#        )
#gtest_discover_tests(huffman_test)


# Unary encoding (practice)
add_executable(unary_coder_micah_practice_test unary_coder_micah_practice_test.cc)
target_link_libraries(unary_coder_micah_practice_test
        PRIVATE contains unary_coder_micah_practice bitvector gtest gtest_main
        )
gtest_discover_tests(unary_coder_micah_practice_test)
