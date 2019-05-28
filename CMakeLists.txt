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
find_package(Boost REQUIRED COMPONENTS system program_options)
include_directories(${Boost_INCLUDE_DIR})

# Include the directories so that finding headers is easier
include_directories(src)
include_directories(.)

set(ACTIONS_FILES src/optimized_actions/optimized_action.cc src/optimized_actions/optimized_action.h src/optimized_actions/action_application.h src/actions/types.h
        src/optimized_actions/arithmetic_operation.cc src/optimized_actions/arithmetic_operation.h src/actions/actions.h src/actions/arithmetic.h)

set(GAME_FILES src/game_components/game_description.h src/game_components/game_state.cc src/game_components/game_state.h
        src/game_components/graph_board.h src/game_components/move.h src/game_components/name_resolver.h src/game_components/search_context.cc
        src/game_components/search_context.h)

set(GRAPH_FILES src/game_nfa/automaton.h src/game_nfa/game_moves_description.h src/game_nfa/graph.h)

set(NETWORKING_FILES src/networking/client.h src/networking/message.h src/networking/networking_utils.cc src/networking/networking_utils.h
        src/networking/server.cc src/networking/server.h)

set(PARSER_FILES src/parser/arithmetic_creator.cc src/parser/arithmetic_creator.h src/parser/game_nfa_creator.cc src/parser/game_nfa_creator.h
        src/parser/parser_actions.h src/parser/parser_connection.cc src/parser/parser_connection.h src/parser/parser_utils.cpp
        src/parser/parser_utils.h src/parser/parser_wrapper.h src/parser/shift_table_creator.cc src/parser/shift_table_creator.h
        src/parser/graph_creator.cc src/parser/graph_creator.h src/parser/actions_creator.cc src/parser/actions_creator.h
        src/parser/arithmetic_operations_creator.cc src/parser/arithmetic_operations_creator.h)

set(UTILS_FILES src/utils/fast_bit_vector.h src/utils/global_config.cc src/utils/utilities.h)

set(PLAYING_FILES ${ACTIONS_FILES} ${GAME_FILES} ${GRAPH_FILES} ${UTILS_FILES})

add_executable(benchmark ${PLAYING_FILES} ${PARSER_FILES} src/benchmark.cc)
target_link_libraries(benchmark rbgParser)
target_link_libraries(benchmark ${Boost_LIBRARIES})
target_link_libraries(benchmark ${CMAKE_THREAD_LIBS_INIT})

add_executable(host ${PLAYING_FILES} ${PARSER_FILES} ${NETWORKING_FILES} src/run_server.cc)
target_link_libraries(host rbgParser)
target_link_libraries(host ${Boost_LIBRARIES})
target_link_libraries(host ${CMAKE_THREAD_LIBS_INIT})

add_executable(random_client ${PLAYING_FILES} ${PARSER_FILES} ${NETWORKING_FILES} src/random_client.cc)
target_link_libraries(random_client rbgParser)
target_link_libraries(random_client ${Boost_LIBRARIES})
target_link_libraries(random_client ${CMAKE_THREAD_LIBS_INIT})

add_executable(material_client ${PLAYING_FILES} ${PARSER_FILES} ${NETWORKING_FILES} src/material_heuristic_client.cc)
target_link_libraries(material_client rbgParser)
target_link_libraries(material_client ${Boost_LIBRARIES})
target_link_libraries(material_client ${CMAKE_THREAD_LIBS_INIT})


enable_testing()
add_executable(graph_test ${GRAPH_FILES} src/tests/graph_test.cc)
add_test(graph graph_test)

add_executable(perft_test ${PLAYING_FILES} ${PARSER_FILES} src/tests/perft_test.cc)
target_link_libraries(perft_test rbgParser)
add_test(perft perft_test)




