# CAPS Labeled Graph Class Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(graph graph.h graph.cc)
target_link_libraries(graph
    PUBLIC node pointer_comparator hash type_debug
)

