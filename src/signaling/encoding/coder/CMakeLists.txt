# CAPS Coder Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(coder INTERFACE)
target_sources(coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/coder.h)

add_library(signed_int_coder INTERFACE)
target_sources(signed_int_coder
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/signed_int_coder.h
)
target_link_libraries(signed_int_coder
    INTERFACE coder
)

add_library(unary_coder INTERFACE)
target_sources(unary_coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/unary_coder.h)
target_link_libraries(unary_coder
    INTERFACE coder bitvector
)

add_library(binary_coder INTERFACE)
target_sources(binary_coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/binary_coder.h)
target_link_libraries(binary_coder
    INTERFACE coder bitvector
)

add_library(gamma_coder INTERFACE)
target_sources(gamma_coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/gamma_coder.h)
target_link_libraries(gamma_coder
    INTERFACE coder unary_coder binary_coder
)

add_library(delta_coder INTERFACE)
target_sources(delta_coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/delta_coder.h)
target_link_libraries(delta_coder
    INTERFACE coder gamma_coder binary_coder
)

add_library(char_coder INTERFACE)
target_sources(char_coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/char_coder.h)
target_link_libraries(char_coder
    INTERFACE coder bitvector defs
)

add_library(string_coder INTERFACE)
target_sources(string_coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/string_coder.h)
target_link_libraries(string_coder
    INTERFACE coder bitvector defs char_coder
)

add_library(huffman_coder INTERFACE)
target_sources(huffman_coder
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/huffman_coder.h
)
target_link_libraries(huffman_coder
    INTERFACE coder delta_coder string_coder
)
