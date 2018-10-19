# FSA Encoding Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(fsa_encoder INTERFACE)
target_sources(fsa_encoder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/fsa_encoder.h)
target_link_libraries(fsa_encoder
    INTERFACE fsa_lexicon graph bitvector contains compare ordering
)

add_library(fsa_huffman_encoder INTERFACE)
target_sources(fsa_huffman_encoder
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/fsa_huffman_encoder.h)
target_link_libraries(fsa_huffman_encoder
    INTERFACE fsa_encoder huffman_coder fsa_lexicon graph bitvector contains
              compare
)
