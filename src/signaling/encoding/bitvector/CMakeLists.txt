# CAPS Bitvector Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(bitvector INTERFACE)
target_sources(bitvector INTERFACE ${CMAKE_CURRENT_LIST_DIR}/bitvector.h)

