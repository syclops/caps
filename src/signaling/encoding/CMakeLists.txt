# CAPS Encoding Library Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(huffman INTERFACE)
target_sources(huffman INTERFACE ${CMAKE_CURRENT_LIST_DIR}/huffman.h)
