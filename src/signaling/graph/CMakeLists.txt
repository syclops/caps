# CAPS Labeled Graph Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

# Use Boost headers for node target
find_package(Boost
    REQUIRED
)
add_library(node node.cc node.h)
target_link_libraries(node
    PUBLIC reverse_iterable
    PRIVATE Boost::boost
)

add_library(graph graph.h graph.cc)
target_link_libraries(graph PUBLIC node)

add_library(visitor visitor.h visitor.cc)
target_link_libraries(visitor PUBLIC node)

add_library(graph_search INTERFACE)
target_sources(graph_search INTERFACE ${CMAKE_CURRENT_LIST_DIR}/graph_search.h)
target_link_libraries(graph_search INTERFACE node graph)

add_library(connected_component connected_component.h connected_component.cc)
target_link_libraries(connected_component PUBLIC node visitor)

add_library(ordering ordering.h ordering.cc)
target_link_libraries(ordering PUBLIC node graph reverse_iterable)