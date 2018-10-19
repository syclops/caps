# CAPS Encoding Library Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(defs INTERFACE)
target_sources(defs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/defs.h)

add_subdirectory(bitvector)

add_library(integer INTERFACE)
target_sources(integer INTERFACE ${CMAKE_CURRENT_LIST_DIR}/integer.h)

add_subdirectory(coder)

add_library(bitvector_io INTERFACE)
target_sources(bitvector_io INTERFACE ${CMAKE_CURRENT_LIST_DIR}/bitvector_io.h)
target_link_libraries(bitvector_io INTERFACE bitvector char_coder)

add_subdirectory(fsa_encoder)
