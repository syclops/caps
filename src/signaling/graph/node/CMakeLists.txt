# CAPS Labeled Graph Node Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

find_package(Boost
    REQUIRED
)

add_library(node node.cc node.h)
target_link_libraries(node
    PUBLIC reverse_iterable
    PRIVATE Boost::boost
)
