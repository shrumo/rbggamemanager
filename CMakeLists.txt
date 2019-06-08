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
add_subdirectory(pybind11)

find_package(Threads)

# See https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIR})

# Include the directories so that finding headers is easier
include_directories(src)
include_directories(.)

set(GRAPH_FILES src/graph/graph.h)

set(PARSER_FILES src/parser/parser_wrapper.h src/parser/parser_actions.h src/parser/parser_wrapper.cc)

set(CONSTRUCTION_FILES
        src/construction/graph_creator.cc src/construction/graph_creator.h)

set(ENV{PYTHONPATH} ${PYTHON_PATH}:${CMAKE_CURRENT_BINARY_DIR})

pybind11_add_module(rbg src/python/rbg.cpp ${GRAPH_FILES} ${PARSER_FILES} ${CONSTRUCTION_FILES})
target_link_libraries(rbg PRIVATE rbgParser)

add_custom_target(visualize_graph ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/graph_widget.py ${CMAKE_CURRENT_BINARY_DIR}/graph_widget.py
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/visualize_graph.py ${CMAKE_CURRENT_BINARY_DIR}/visualize_graph.py
        DEPENDS rbg)

# There is a bug in CMake, so that in order to run tests you have to build executables manually
enable_testing()
add_executable(graph_test ${GRAPH_FILES} src/tests/graph_test.cc)
add_test(graph graph_test)
set_property(TEST graph APPEND PROPERTY DEPENDS graph_test)

add_executable(parser_test ${GRAPH_FILES} ${PARSER_FILES} src/tests/parser_test.cc)
target_link_libraries(parser_test rbgParser)
add_test(parser parser_test)
set_property(TEST parser APPEND PROPERTY DEPENDS parser_test)

add_executable(creator_test ${GRAPH_FILES} ${PARSER_FILES} ${CONSTRUCTION_FILES} src/tests/creator_test.cc)
target_link_libraries(creator_test rbgParser)
add_test(creator creator_test)
set_property(TEST creator APPEND PROPERTY DEPENDS creator_test)

add_custom_target(python_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/python_test.py ${CMAKE_CURRENT_BINARY_DIR}/python_test.py
        DEPENDS rbg)
add_test(python ${PYTHON_EXECUTABLE} python_test.py)






