# cxxopts Dependency Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

if (NOT cxxopts_POPULATED)
  FetchContent_Populate(cxxopts)
  add_subdirectory(${cxxopts_SOURCE_DIR} ${cxxopts_BINARY_DIR})
endif()