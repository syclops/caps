# CAPS Labeled Graph Traversal Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(graph_search INTERFACE)
target_sources(graph_search INTERFACE ${CMAKE_CURRENT_LIST_DIR}/graph_search.h)
target_link_libraries(graph_search INTERFACE node graph)

