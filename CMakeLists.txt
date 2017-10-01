# Standard project metadata
cmake_minimum_required(VERSION 2.8.2)
project(caps)

# Set the output directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Use C++11 and set desired compile flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -pedantic -g")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -pedantic -Ofast")

# Use Boost
# Note: you need to change the Boost_INCLUDE_DIR variable to wherever your local
#       Boost library is located.
find_package(Boost 1.64 COMPONENTS program_options REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_INCLUDE_DIR /usr/local/Cellar/boost/1.64.0_1/include)
include_directories(${Boost_INCLUDE_DIR})

if (CMAKE_VERSION VERSION_LESS 3.2)
  set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
  set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

# Use crascit's DownloadProject framework to build external libraries.
#include(DownloadProject.cmake)

# Integrate GoogleTest for unit tests
#download_project(PROJ googletest
#  GIT_REPOSITORY      https://github.com/google/googletest.git
#  GIT_TAG             master
#  ${UPDATE_DISCONNECTED_IF_AVAILABLE}
#)

# Use SDSL-Lite for succinct data structures
#download_project(PROJ sdsl-lite
#  GIT_REPOSITORY https://github.com/simongog/sdsl-lite.git
#  GIT_TAG master
#  ${UPDATE_DISCONNECTED_IF_AVAILABLE}
#)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

#add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to the gtest and gmock targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
#if (CMAKE_VERSION VERSION_LESS 2.8.11)
#  include_directories(
#    "${gtest_SOURCE_DIR}/include"
#    "${gmock_SOURCE_DIR}/include"
#  )
#endif()

# Set the source and testing directories for C++ code
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/cpp)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test/src)

# Declare the relevant subdirectories
set(SIGNAL_AUTH_DIR ${SRC_DIR}/signal_authority)
set(SERVER_DIR ${SRC_DIR}/server)
set(CLIENT_DIR ${SRC_DIR}/client)
set(ENCODING_DIR ${SRC_DIR}/encoding)

# Declare the files used for writing bitstreams
set(BITSTREAM_FILES
  ${ENCODING_DIR}/bitstream.h
  ${ENCODING_DIR}/bitstream.cc
)

# Declare the files used for nodes in the trie and DAFSA graphs
set(NODE_FILES
  ${SIGNAL_AUTH_DIR}/node.h
  ${SIGNAL_AUTH_DIR}/node.cc
)

# Declare the files used for the graph base class (from which the trie and DAFSA
# classes derive)
set(GRAPH_BASE_FILES
  ${SIGNAL_AUTH_DIR}/graphbase.h
  ${SIGNAL_AUTH_DIR}/graphbase.cc
  )

# Declare the base files used for the signal authority
set(AUTH_BASE_FILES
  ${NODE_FILES}
  ${GRAPH_BASE_FILES}
)

# Declare the files for the signal authority's Huffman encoder
set(AUTH_HUFFMAN_FILES
  ${BITSTREAM_FILES}
  ${SIGNAL_AUTH_DIR}/huffman.h
  ${SIGNAL_AUTH_DIR}/huffman.cc
)

# Declare the files for the DAFSA-based signal authority
set(AUTH_MAFSA_FILES
  ${BITSTREAM_FILES}
  ${AUTH_BASE_FILES}
  ${AUTH_HUFFMAN_FILES}
  ${SIGNAL_AUTH_DIR}/mafsa.h
  ${SIGNAL_AUTH_DIR}/mafsa.cc
)

# Declare the files used for the trie-based implementation of the signal
# authority
set(AUTH_TRIE_FILES
  ${AUTH_BASE_FILES}
  ${SIGNAL_AUTH_DIR}/trie.h
  ${SIGNAL_AUTH_DIR}/trie.cc
  ${SIGNAL_AUTH_DIR}/triebuild.cc
)

# Declare the files used to build the Huffman tree in isolation (for size
# estimation)
set(HUFFMAN_BUILD_FILES
  ${AUTH_HUFFMAN_FILES}
  ${SIGNAL_AUTH_DIR}/huffmanbuild.cc
)

# Declare the files used to build the debug version of the DAFSA (with testing
# and analysis functionality)
set(MAFSA_DEBUG_FILES
  ${AUTH_MAFSA_FILES}
  ${TEST_DIR}/mafsadebug.h
  ${TEST_DIR}/mafsadebug.cc
)

# Declare the files used to build a DAFSA by the signal authority
set(AUTH_MAFSA_BUILD_FILES
  ${MAFSA_DEBUG_FILES}
  ${SIGNAL_AUTH_DIR}/mafsabuild.cc
)

# Declare the files used to build the debug version of the signal authrority's
# Huffman encoder
set(HUFFMAN_DEBUG_FILES
  ${AUTH_HUFFMAN_FILES}
  ${TEST_DIR}/huffmandebug.h
  ${TEST_DIR}/huffmandebug.cc
)

# Declare the files used to evaluate the signal authority's DAFSA with various
# optimizations
set(MAFSA_EVAL_FILES
  ${MAFSA_DEBUG_FILES}
  ${TEST_DIR}/mafsaeval.cc
)

# Declare the files used to write the label counts of a DAFSA to a file
set(MAFSA_LABEL_FILES
  ${MAFSA_DEBUG_FILES}
  ${TEST_DIR}/mafsalabel.cc
)

# Declare files used to estimate the size of a Huffman tree
set(HUFFMAN_TEST_FILES
  ${HUFFMAN_DEBUG_FILES}
  ${TEST_DIR}/huffmantest.cc
)

# Declare executables and their dependencies
add_executable(triebuild ${AUTH_TRIE_FILES})
add_executable(mafsabuild ${AUTH_MAFSA_BUILD_FILES})
target_link_libraries(mafsabuild ${Boost_LIBRARIES})
add_executable(huffmanbuild ${HUFFMAN_BUILD_FILES})
add_executable(huffmantest ${HUFFMAN_TEST_FILES})
add_executable(mafsaeval ${MAFSA_EVAL_FILES})
add_executable(mafsalabel ${MAFSA_LABEL_FILES})

#install(TARGETS triebuild mafsabuild huffmanbuild DESTINATION build)
#install(TARGETS huffmantest nodetest DESTINATION test)
