# CAPS Encoding Library Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(defs INTERFACE)
target_sources(defs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/defs.h)

add_subdirectory(bitvector)

add_subdirectory(coder)

add_library(integer INTERFACE)
target_sources(integer INTERFACE ${CMAKE_CURRENT_LIST_DIR}/integer.h)

add_library(huffman INTERFACE)
target_sources(huffman INTERFACE ${CMAKE_CURRENT_LIST_DIR}/huffman.h)
target_link_libraries(huffman INTERFACE integer)

add_library(fsa_encoder fsa_encoder.h)
target_link_libraries(fsa_encoder
    PUBLIC fsa_lexicon graph sdsl
    PRIVATE bitvector contains compare
)
