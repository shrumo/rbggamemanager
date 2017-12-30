cmake_minimum_required(VERSION 3.7)
project(rbggamemanager)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(rbg2gdl/)

find_package(Threads)

# see https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIR})

set(SOURCE_FILES ${SOURCE_FILES} main.cpp automaton.cpp automaton.h game.cpp game.h action.cpp action.h)
add_executable(rbggamemanager ${SOURCE_FILES})
target_link_libraries(rbggamemanager parser)
target_link_libraries(rbggamemanager ${Boost_LIBRARIES})
target_link_libraries(rbggamemanager ${CMAKE_THREAD_LIBS_INIT})