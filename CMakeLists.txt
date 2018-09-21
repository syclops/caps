# CAPS CMake Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

# Preamble
cmake_minimum_required(VERSION 3.12)
project(caps)
enable_testing()

# Project-wide setup
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -pedantic -g")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -pedantic -O3")

# External dependencies
add_subdirectory(dependencies)

# Project targets
add_subdirectory(src)

# If this is the top-level project, include the tests and packaging configs.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  add_subdirectory(tests)
  add_subdirectory(packaging)
endif()

