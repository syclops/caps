# CAPS Labeled Graph Visitor Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(visitor visitor.h visitor.cc)
target_link_libraries(visitor PUBLIC node)
