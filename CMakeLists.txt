cmake_minimum_required(VERSION 3.5.0)
project(rbggamemanager)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXXFLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11 -O3 -flto -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic -O0 -std=c++11 -g")

add_subdirectory(rbgParser)

find_package(Threads)

# see https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost REQUIRED COMPONENTS system program_options)
include_directories(${Boost_INCLUDE_DIR})

set(SOURCE_FILES ${SOURCE_FILES} src/main.cpp src/game_nfa/automaton.h src/game_components/name_resolver.h src/actions/action.cpp src/actions/action.h src/game_components/game_description.h src/game_nfa/game_moves_description.h src/game_components/game_state.cpp src/game_components/game_state.h src/game_components/board.h src/game_components/lazy_evaluator.cpp src/game_components/lazy_evaluator.h src/actions/condition.cpp src/actions/condition.h src/actions/arithmetic_operation.cpp src/actions/arithmetic_operation.h src/parser_connection/game_nfa_creator.cpp src/parser_connection/game_nfa_creator.h src/parser_connection/parser_connection.cpp src/parser_connection/parser_connection.h src/game_components/moves_cache.cpp src/game_components/moves_cache.h)
add_executable(rbggamemanager ${SOURCE_FILES})
target_link_libraries(rbggamemanager rbgParser)
target_link_libraries(rbggamemanager ${Boost_LIBRARIES})
target_link_libraries(rbggamemanager ${CMAKE_THREAD_LIBS_INIT})