# CAPS Coder Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(coder INTERFACE)
target_sources(coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/coder.h)
target_link_libraries(coder INTERFACE bitvector)

add_library(unary_coder INTERFACE)
target_sources(unary_coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/unary_coder.h)
target_link_libraries(unary_coder INTERFACE coder bitvector)

add_library(char_coder INTERFACE)
target_sources(char_coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/char_coder.h)
target_link_libraries(char_coder INTERFACE coder bitvector defs)

add_library(string_coder INTERFACE)
target_sources(string_coder INTERFACE ${CMAKE_CURRENT_LIST_DIR}/string_coder.h)
target_link_libraries(string_coder INTERFACE coder bitvector defs char_coder)
