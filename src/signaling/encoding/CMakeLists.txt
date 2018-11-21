# CAPS Encoding Library Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_subdirectory(coder)

add_library(bitvector_io INTERFACE)
target_sources(bitvector_io INTERFACE ${CMAKE_CURRENT_LIST_DIR}/bitvector_io.h)
target_link_libraries(bitvector_io INTERFACE bitvector char_coder)

add_subdirectory(fsa_encoder)
