cmake_minimum_required(VERSION 3.5.0)
project(rbggamemanager)

# Build in release by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pipe")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -march=native")

add_subdirectory(rbgParser)

find_package(Threads)

# See https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIR})

# Include the directories so that finding headers is easier
include_directories(src)
include_directories(.)

set(GRAPH_FILES src/graph/graph.h)

set(PARSER_FILES src/parser/parser_wrapper.h src/parser/parser_actions.h)


enable_testing()
add_executable(graph_test ${GRAPH_FILES} src/tests/graph_test.cc)
add_test(graph graph_test)

add_executable(parser_test ${GRAPH_FILES} ${PARSER_FILES} src/tests/parser_test.cc)
target_link_libraries(parser_test rbgParser)
add_test(parser parser_test)




