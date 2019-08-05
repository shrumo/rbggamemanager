cmake_minimum_required(VERSION 3.5.0)
project(rbggamemanager)

# Build in release by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pipe -Ofast")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -march=native")

add_subdirectory(rbgParser)
add_subdirectory(pybind11)

find_package(Threads)

include_directories(asio/asio/include)

# Include the directories so that finding headers is easier
include_directories(src)
include_directories(.)

set(GRAPH_FILES src/utility/graph.h src/stl_extension/range.h src/stl_extension/utility.h src/stl_extension/iterator_transform.h)

set(PARSER_FILES src/parser/parser_wrapper.h src/parser/parser_actions.h src/parser/parser_wrapper.cc)

set(GAMESTATE_FILES ${PARSER_FILES} ${GRAPH_FILES} src/game_description/names_resolver.h src/game_description/board.h src/game_state/blocks/block.h
        src/game_description/declarations.h src/game_state/game_state.h src/utility/resetabble_bitarray_stack.h
        src/game_description/moves/arithmetic_operation.cc src/game_description/moves/arithmetic_operation.h
        src/utility/arithmetic_operation_visitor.h
        src/game_description/names_resolver.cc src/game_description/board.cc
        src/utility/calculate_perft.h
        src/game_state/blocks/actions.h src/stl_extension/vector.h
        src/game_state/blocks/modifying_application.h src/game_state/blocks/modifying_application.cpp
        src/game_description/construction/graph_creator.cc src/game_description/construction/graph_creator.h
        src/game_description/moves/moves.h src/game_description/moves/moves.cc
        src/game_description/construction/moves_creator.cc src/game_description/construction/moves_creator.h
        src/game_description/construction/arithmetic_creator.cc src/game_description/construction/arithmetic_creator.h
        src/utility/moves_visitor.h src/utility/printer.cc
        src/utility/printer.h src/game_state/construction/blocks_creator.cc src/game_state/construction/blocks_creator.h
        src/game_description/moves/arithmetic_operation.cc src/game_description/moves/arithmetic_operation.h
        src/game_description/board.cc src/game_state/blocks/block.h src/game_state/construction/game_state_creator.h
        src/game_state/construction/game_state_creator.cc
        )

set(NETWORKING_FILES ${GAMESTATE_FILES} src/networking/server.h src/networking/socket.h src/networking/client.h src/networking/socket.cc)

set(ENV{PYTHONPATH} ${PYTHON_PATH}:${CMAKE_CURRENT_BINARY_DIR})

pybind11_add_module(rbg src/python_bindings/rbg.cc ${GAMESTATE_FILES})
target_link_libraries(rbg PRIVATE rbgParser)

add_custom_target(visualize_graph ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/python_widgets/graph_widget.py ${CMAKE_CURRENT_BINARY_DIR}/graph_widget.py
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/python_widgets/visualize_graph.py ${CMAKE_CURRENT_BINARY_DIR}/visualize_graph.py
        DEPENDS rbg)

add_executable(benchmark ${GRAPH_FILES} ${PARSER_FILES}
        ${CONSTRUCTION_FILES} ${GAMESTATE_FILES} src/benchmark.cc)

target_link_libraries(benchmark rbgParser)

add_executable(explain ${GRAPH_FILES} ${PARSER_FILES}
        ${CONSTRUCTION_FILES} ${GAMESTATE_FILES} src/explain.cc)

target_link_libraries(explain rbgParser)

add_executable(start_server ${NETWORKING_FILES} src/start_server.cc)
target_link_libraries(start_server rbgParser)
target_link_libraries(start_server pthread)

add_executable(start_random_client ${NETWORKING_FILES} src/start_random_client.cc)
target_link_libraries(start_random_client rbgParser)
target_link_libraries(start_random_client pthread)

# There is a bug in CMake, so that in order to run tests you have to build executables manually
enable_testing()
add_executable(graph_test ${GRAPH_FILES} src/tests/graph_test.cc)
add_test(graph_test graph_test)

add_executable(parser_test ${GRAPH_FILES} ${PARSER_FILES} src/tests/parser_test.cc)
target_link_libraries(parser_test rbgParser)
add_test(parser_test parser_test)

add_executable(creator_test ${GAMESTATE_FILES} src/tests/creator_test.cc)
target_link_libraries(creator_test rbgParser)
add_test(creator_test creator_test)

add_executable(searchsteps_test  ${GAMESTATE_FILES} src/tests/single_blocks_test.cc)
target_link_libraries(searchsteps_test rbgParser)
add_test(searchsteps_test searchsteps_test)

add_executable(declarations_test  ${GAMESTATE_FILES} src/tests/declarations_test.cc)
target_link_libraries(declarations_test rbgParser)
add_test(declarations_test declarations_test)

add_executable(namesresolver_test src/game_description/names_resolver.h src/game_description/names_resolver.cc src/tests/namesresolver_test.cc)
add_test(namesresolver_test namesresolver_test)

add_executable(gamestate_simple_test  ${GAMESTATE_FILES} src/tests/game_state_simple_test.cc)
target_link_libraries(gamestate_simple_test rbgParser)
add_test(gamestate_simple_test gamestate_simple_test)

add_executable(chess_test ${GAMESTATE_FILES} src/tests/chess_test.cc)
target_link_libraries(chess_test rbgParser)
add_test(chess_test chess_test)

add_executable(board_test src/game_description/board.h src/game_description/board.cc src/tests/board_test.cc)
add_test(board_test board_test)

add_custom_target(python_bindings_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/python_bindings_test.py ${CMAKE_CURRENT_BINARY_DIR}/python_bindings_test.py
        DEPENDS rbg)
add_test(python_bindings_test ${PYTHON_EXECUTABLE} python_bindings_test.py)






