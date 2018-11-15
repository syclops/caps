# CAPS Labeled Graph Connected Component Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(connected_component
    connected_component.h
    connected_component.cc
)
target_link_libraries(connected_component
    PUBLIC node
)

add_library(transitive_path_visitor
    transitive_path_visitor.h
    transitive_path_visitor.cc
)
target_link_libraries(transitive_path_visitor
    PUBLIC node visitor connected_component
)

add_library(connected_component_utils
    connected_component_utils.h
    connected_component_utils.cc
)
target_link_libraries(connected_component_utils
    PUBLIC connected_component transitive_path_visitor
    PRIVATE graph_search
)