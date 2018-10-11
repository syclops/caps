# Succinct Data Structures Library Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

if (NOT sdsl_POPULATED)
  FetchContent_Populate(sdsl)
  add_subdirectory(${sdsl_SOURCE_DIR} ${sdsl_BINARY_DIR})
endif()
