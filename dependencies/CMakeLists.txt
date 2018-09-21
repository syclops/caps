# CAPS Dependencies Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

# Import module to allow easy dependency configuration
include(FetchContent)

FetchContent_Declare(cxxopts
  GIT_REPOSITORY      https://github.com/jarro2783/cxxopts.git
  GIT_TAG             master
)
add_subdirectory(cxxopts)

FetchContent_Declare(gtest
  GIT_REPOSITORY      https://github.com/google/googletest.git
  GIT_TAG             master
)
add_subdirectory(gtest)

