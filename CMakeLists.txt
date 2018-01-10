cmake_minimum_required(VERSION 3.5.0)
project(rbggamemanager)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(rbgParser/)

find_package(Threads)

# see https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost REQUIRED COMPONENTS system program_options)
include_directories(${Boost_INCLUDE_DIR})

set(SOURCE_FILES ${SOURCE_FILES} main.cpp automaton.h action.cpp action.h game_state.cpp game_state.h board.h name_resolver.h dispatchers.cpp dispatchers.h)
add_executable(rbggamemanager ${SOURCE_FILES})
target_link_libraries(rbggamemanager rbgParser)
target_link_libraries(rbggamemanager ${Boost_LIBRARIES})
target_link_libraries(rbggamemanager ${CMAKE_THREAD_LIBS_INIT})