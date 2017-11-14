# Standard project metadata
cmake_minimum_required(VERSION 3.5)
project(caps)

# Set the output directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Use C++11 and set desired compile flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -pedantic -g")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -pedantic -pg -Ofast")

# Use Boost
# Note: you need to change the Boost_INCLUDE_DIR variable to wherever your local
#       Boost library is located.
#find_package(Boost)
#if(Boost_FOUND)
#  include_directories(${Boost_INCLUDE_DIR})
#else()
#  message(FATAL_ERROR "Boost not found")
#endif()

# Use crascit's DownloadProject framework to build external libraries.
include(DownloadProject.cmake)

# Integrate GoogleTest for unit tests
download_project(PROJ googletest
    GIT_REPOSITORY      https://github.com/google/googletest.git
    GIT_TAG             master
    ${UPDATE_DISCONNECTED_IF_AVAILABLE}
    )

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

download_project(PROJ cxxopts
  GIT_REPOSITORY      https://github.com/jarro2783/cxxopts.git
  GIT_TAG             master
  ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

add_subdirectory(${cxxopts_SOURCE_DIR} ${cxxopts_BINARY_DIR})

# Use SDSL-Lite for succinct data structures
#download_project(PROJ sdsl-lite
#  GIT_REPOSITORY https://github.com/simongog/sdsl-lite.git
#  GIT_TAG master
#  ${UPDATE_DISCONNECTED_IF_AVAILABLE}
#)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Declare executables and their dependencies

add_executable(fsa_lexicon tools/cpp/fsa_lexicon_tool.cc
    src/cpp/graph/node.h src/cpp/graph/node.cc
    src/cpp/graph/visitor.h src/cpp/graph/visitor.cc
    src/cpp/graph/graph.h src/cpp/graph/graph.cc
    src/cpp/lexicon/fsa_lexicon.h src/cpp/lexicon/fsa_lexicon.cc
    src/cpp/lexicon/lexicon.h src/cpp/lexicon/lexicon.cc
    src/cpp/graph/visitor.cc src/cpp/graph/visitor.h
    src/cpp/graph/connected_component.cc src/cpp/graph/connected_component.h
    src/cpp/common/iterable.h src/cpp/common/contains.h
    src/cpp/graph/graph_search.h src/cpp/common/powerset.h)
target_include_directories(fsa_lexicon INTERFACE ${cxxopts_SOURCE_DIR})

add_executable(nodetest
        src/cpp/graph/node.h
        src/cpp/graph/node.cc
    test/src/node_test.cc src/cpp/common/iterable.h src/cpp/common/contains.h src/cpp/graph/graph_search.h)
target_link_libraries(nodetest gtest gtest_main)

add_executable(graphbasetest
        src/cpp/graph/node.h
        src/cpp/graph/node.cc
        src/cpp/graph/graph.h
        src/cpp/graph/graph.cc
        test/src/graphbase_test.cc src/cpp/graph/visitor.cc src/cpp/graph/visitor.h src/cpp/common/iterable.h src/cpp/common/contains.h src/cpp/graph/graph_search.h)
target_link_libraries(graphbasetest gtest gtest_main)

add_executable(set_lexicon_test
        src/cpp/lexicon/lexicon.h
        src/cpp/lexicon/lexicon.cc
        src/cpp/lexicon/set_lexicon.h
        src/cpp/lexicon/set_lexicon.cc
    test/src/set_lexicon_test.cc src/cpp/common/contains.h)
target_link_libraries(set_lexicon_test gtest gtest_main)

add_executable(fsa_lexicon_test
    src/cpp/graph/node.h src/cpp/graph/node.cc
    src/cpp/graph/visitor.h src/cpp/graph/visitor.cc
    src/cpp/graph/graph.h src/cpp/graph/graph.cc
    src/cpp/lexicon/lexicon.h src/cpp/lexicon/lexicon.cc
    src/cpp/lexicon/fsa_lexicon.h src/cpp/lexicon/fsa_lexicon.cc
    test/fsa_lexicon/fsa_lexicon_test.cc src/cpp/graph/visitor.cc src/cpp/graph/visitor.h src/cpp/graph/connected_component.cc src/cpp/graph/connected_component.h src/cpp/common/iterable.h src/cpp/common/contains.h src/cpp/graph/graph_search.h src/cpp/common/powerset.h test/fsa_lexicon/test_helper.h)
target_link_libraries(fsa_lexicon_test gtest gtest_main)

add_executable(lexicon_test
    src/cpp/graph/node.h src/cpp/graph/node.cc
    src/cpp/graph/visitor.h src/cpp/graph/visitor.cc
    src/cpp/graph/graph.h src/cpp/graph/graph.cc
    src/cpp/lexicon/lexicon.h src/cpp/lexicon/lexicon.cc
    src/cpp/lexicon/set_lexicon.h src/cpp/lexicon/set_lexicon.cc
    src/cpp/lexicon/fsa_lexicon.h src/cpp/lexicon/fsa_lexicon.cc
    src/cpp/graph/visitor.cc src/cpp/graph/visitor.h test/src/lexicon_test.cc src/cpp/common/iterable.h src/cpp/common/contains.h src/cpp/graph/graph_search.h)
target_link_libraries(lexicon_test gtest gtest_main)

add_executable(connected_component_test
    src/cpp/graph/node.cc src/cpp/graph/node.h
    src/cpp/graph/graph.cc src/cpp/graph/graph.h
    src/cpp/graph/visitor.cc src/cpp/graph/visitor.h
    src/cpp/graph/connected_component.cc src/cpp/graph/connected_component.h
    test/src/connected_component_test.cc src/cpp/common/iterable.h src/cpp/common/contains.h)
target_link_libraries(connected_component_test gtest gtest_main)

add_executable(powerset_test src/cpp/common/powerset.h test/src/powerset_test.cc)
target_link_libraries(powerset_test gtest gtest_main)

add_executable(lexicon_run
    src/cpp/lexicon/lexicon_run.cc src/cpp/graph/graph_search.h
    src/cpp/lexicon/lexicon.h src/cpp/lexicon/lexicon.cc
    src/cpp/lexicon/fsa_lexicon.h src/cpp/lexicon/fsa_lexicon.cc
    src/cpp/graph/node.h src/cpp/graph/node.cc
    src/cpp/common/contains.h src/cpp/common/iterable.h
    src/cpp/common/powerset.h
    src/cpp/graph/visitor.h src/cpp/graph/visitor.cc
    src/cpp/graph/connected_component.h src/cpp/graph/connected_component.cc
    src/cpp/graph/graph.h src/cpp/graph/graph.cc)

add_executable(graph_test
  src/cpp/graph/node.h src/cpp/graph/node.cc src/cpp/graph/graph.h
  src/cpp/graph/graph.cc src/cpp/graph/visitor.cc src/cpp/graph/visitor.h src/cpp/common/iterable.h src/cpp/common/contains.h src/cpp/graph/graph_search.h)

add_executable(triebuild
  src/cpp/graph/node.h src/cpp/graph/node.cc src/cpp/graph/graph.h
  src/cpp/graph/graph.cc src/cpp/signal_authority/trie.h
  src/cpp/signal_authority/trie.cc src/cpp/signal_authority/triebuild.cc)

add_executable(mafsabuild
  src/cpp/encoding/bitstream.h src/cpp/encoding/bitstream.cc
  src/cpp/graph/node.h src/cpp/graph/node.cc
  src/cpp/graph/graph.h src/cpp/graph/graph.cc
  src/cpp/encoding/huffman.h src/cpp/encoding/huffman.cc
  src/cpp/signal_authority/mafsa.h src/cpp/signal_authority/mafsa.cc
  test/src/mafsadebug.h test/src/mafsadebug.cc
  src/cpp/signal_authority/mafsabuild.cc)
target_link_libraries(mafsabuild ${Boost_LIBRARIES})

add_executable(huffmanbuild
  src/cpp/encoding/bitstream.h
  src/cpp/encoding/bitstream.cc
        src/cpp/encoding/huffman.h
        src/cpp/encoding/huffman.cc
  src/cpp/signal_authority/huffmanbuild.cc)

add_executable(huffman_test
  src/cpp/encoding/bitstream.h
  src/cpp/encoding/bitstream.cc
        src/cpp/encoding/huffman.h
        src/cpp/encoding/huffman.cc
  src/cpp/common/util.h
  test/src/huffmandebug.h
  test/src/huffmandebug.cc
        test/src/huffman_test.cc)

add_executable(mafsaeval
  src/cpp/encoding/bitstream.h
  src/cpp/encoding/bitstream.cc
        src/cpp/encoding/huffman.h
        src/cpp/graph/node.h
        src/cpp/graph/node.cc
        src/cpp/graph/graph.h
        src/cpp/graph/graph.cc
        src/cpp/encoding/huffman.h
        src/cpp/encoding/huffman.cc
  src/cpp/signal_authority/mafsa.h
  src/cpp/signal_authority/mafsa.cc
  test/src/mafsadebug.h
  test/src/mafsadebug.cc
  test/src/mafsaeval.cc)

add_executable(mafsalabel
  src/cpp/encoding/bitstream.h
  src/cpp/encoding/bitstream.cc
        src/cpp/encoding/huffman.h
        src/cpp/graph/node.h
        src/cpp/graph/node.cc
        src/cpp/graph/graph.h
        src/cpp/graph/graph.cc
        src/cpp/encoding/huffman.h
        src/cpp/encoding/huffman.cc
  src/cpp/signal_authority/mafsa.h
  src/cpp/signal_authority/mafsa.cc
  test/src/mafsadebug.h
  test/src/mafsadebug.cc
  test/src/mafsalabel.cc)

#install(TARGETS triebuild mafsabuild huffmanbuild DESTINATION build)
#install(TARGETS huffmantest nodetest DESTINATION test)
