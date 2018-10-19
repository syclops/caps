# CAPS Signaling Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_subdirectory(common)
add_subdirectory(graph)
add_subdirectory(lexicon)
add_subdirectory(encoding)

add_executable(build_lexicon build_lexicon.cc)
target_link_libraries(build_lexicon
    PUBLIC fsa_lexicon
    PRIVATE cxxopts io_option measure_time bitvector_io fsa_encoder
            fsa_huffman_encoder
)