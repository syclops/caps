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

FetchContent_Declare(sdsl
  GIT_REPOSITORY https://github.com/simongog/sdsl-lite.git
  GIT_TAG 0546faf0552142f06ff4b201b671a5769dd007ad # Release 2.1.1
)
#add_subdirectory(sdsl)
