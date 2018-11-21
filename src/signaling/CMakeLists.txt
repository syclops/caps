# CAPS Signaling Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_subdirectory(common)
add_subdirectory(graph)
add_subdirectory(lexicon)
add_subdirectory(encoding)

add_executable(build_lexicon build_lexicon.cc)
target_link_libraries(build_lexicon
    PUBLIC bitvector fsa_lexicon bitvector_io
    PRIVATE cxxopts io_option measure_time fsa_encoder fsa_huffman_encoder
)