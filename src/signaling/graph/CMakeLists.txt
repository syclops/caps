# CAPS Labeled Graph Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_subdirectory(node)

add_subdirectory(visitor)

add_subdirectory(traversal)

add_subdirectory(component)

add_subdirectory(labeled_graph)

add_library(ordering ordering.h ordering.cc)
target_link_libraries(ordering
    PUBLIC reverse_iterable node graph
)

