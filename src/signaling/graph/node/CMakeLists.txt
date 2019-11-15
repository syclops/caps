# CAPS Labeled Graph Node Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

find_package(Boost
    REQUIRED
)

message("Boost include: ${Boost_INCLUDE_DIRS}")
message("Boost include: ${Boost_INCLUDE_DIR}")
message("Boost library: ${Boost_LIBRARY_DIRS}")

add_library(node node.cc node.h)
target_link_libraries(node
    PUBLIC reverse_iterable
    PRIVATE Boost::boost
)
