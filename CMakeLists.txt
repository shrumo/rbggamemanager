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

set(GAMESTATE_FILES src/game_state/namesresolver.h src/game_state/board.h src/game_state/searchsteps/search_step.h
        src/game_state/declarations.h src/game_state/game_state.h src/game_state/searchsteps/resetabblebitarraystack.h
        src/game_state/searchsteps/arithmetic_operation.cc src/game_state/searchsteps/arithmetic_operation.h
        src/game_state/searchsteps/arithmetic_operation_visitor.h
        src/game_state/namesresolver.cc src/game_state/board.cc src/game_state/searchsteps/search_step.cc)

set(GRAPH_FILES src/graph/graph.h)

set(PARSER_FILES src/parser/parser_wrapper.h src/parser/parser_actions.h src/parser/parser_wrapper.cc)

set(CONSTRUCTION_FILES
        src/construction/graph_creator.cc src/construction/graph_creator.h src/construction/declarations_creator.h
        src/construction/moves/moves.h src/construction/moves/moves_creator.cc src/construction/moves/moves_creator.h
        src/construction/moves/arithmetic_creator.cc src/construction/moves/arithmetic_creator.h
        src/construction/moves/moves_visitor.h src/construction/moves/moves_printer.cc
        src/construction/moves/moves_printer.h src/construction/searchstep_creator.cc src/construction/searchstep_creator.h
        src/game_state/searchsteps/arithmetic_operation.cc src/game_state/searchsteps/arithmetic_operation.h
        src/game_state/board.cc src/game_state/searchsteps/search_step.cc src/game_state/searchsteps/search_step.h src/construction/gamestate_creator.h)

set(ENV{PYTHONPATH} ${PYTHON_PATH}:${CMAKE_CURRENT_BINARY_DIR})

pybind11_add_module(rbg src/python/rbg.cc ${GRAPH_FILES} ${PARSER_FILES} ${CONSTRUCTION_FILES})
target_link_libraries(rbg PRIVATE rbgParser)

add_custom_target(visualize_graph ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/graph_widget.py ${CMAKE_CURRENT_BINARY_DIR}/graph_widget.py
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities/visualize_graph.py ${CMAKE_CURRENT_BINARY_DIR}/visualize_graph.py
        DEPENDS rbg)

# There is a bug in CMake, so that in order to run tests you have to build executables manually
enable_testing()
add_executable(graph_test ${GRAPH_FILES} src/tests/graph_test.cc)
add_test(graph_test graph_test)

add_executable(parser_test ${GRAPH_FILES} ${PARSER_FILES} src/tests/parser_test.cc)
target_link_libraries(parser_test rbgParser)
add_test(parser_test parser_test)

add_executable(creator_test ${GRAPH_FILES} ${PARSER_FILES} ${CONSTRUCTION_FILES} ${GAMESTATE_FILES} src/tests/creator_test.cc)
target_link_libraries(creator_test rbgParser)
add_test(creator_test creator_test)

add_executable(declarations_test ${PARSER_FILES} ${CONSTRUCTION_FILES} ${GAMESTATE_FILES} src/tests/declarations_test.cc)
target_link_libraries(declarations_test rbgParser)
add_test(declarations_test declarations_test)

add_executable(namesresolver_test ${GAMESTATE_FILES} src/tests/namesresolver_test.cc)
add_test(namesresolver_test namesresolver_test)

add_executable(gamestate_simple_test ${GRAPH_FILES} ${PARSER_FILES} ${CONSTRUCTION_FILES} ${GAMESTATE_FILES} src/tests/gamestate_simple_test.cc)
target_link_libraries(gamestate_simple_test rbgParser)
add_test(gamestate_simple_test gamestate_simple_test)

add_executable(board_test ${GAMESTATE_FILES} src/tests/board_test.cc)
add_test(board_test board_test)

add_custom_target(python_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/python_test.py ${CMAKE_CURRENT_BINARY_DIR}/python_test.py
        DEPENDS rbg)
add_test(python_test ${PYTHON_EXECUTABLE} python_test.py)






