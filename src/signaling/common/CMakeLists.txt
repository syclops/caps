# CAPS Common Utilities Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(contains INTERFACE)
target_sources(contains INTERFACE ${CMAKE_CURRENT_LIST_DIR}/contains.h)

add_library(compare INTERFACE)
target_sources(compare INTERFACE ${CMAKE_CURRENT_LIST_DIR}/compare.h)

add_library(io_option INTERFACE)
target_sources(io_option INTERFACE ${CMAKE_CURRENT_LIST_DIR}/io_option.h)

add_library(measure_time INTERFACE)
target_sources(measure_time INTERFACE ${CMAKE_CURRENT_LIST_DIR}/measure_time.h)

add_library(powerset INTERFACE)
target_sources(powerset INTERFACE ${CMAKE_CURRENT_LIST_DIR}/powerset.h)

add_library(reverse_iterable INTERFACE)
target_sources(reverse_iterable INTERFACE ${CMAKE_CURRENT_LIST_DIR}/iterable.h)
