# CAPS Lexicon Unit Test Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

include(GoogleTest)

# Lexicon
add_executable(lexicon_test lexicon_test.cc)
target_link_libraries(lexicon_test
    PUBLIC set_lexicon fsa_lexicon
    PRIVATE lexicon gtest gtest_main
)
gtest_discover_tests(lexicon_test)

# Set Lexicon
add_executable(set_lexicon_test set_lexicon_test.cc)
target_link_libraries(set_lexicon_test
    PRIVATE set_lexicon gtest gtest_main
)
gtest_discover_tests(set_lexicon_test)

# DAFSA Lexicon
add_executable(fsa_lexicon_test fsa_lexicon_test.cc)
target_link_libraries(fsa_lexicon_test
    PUBLIC node fsa_lexicon
    PRIVATE gtest gtest_main
)
gtest_discover_tests(fsa_lexicon_test)
