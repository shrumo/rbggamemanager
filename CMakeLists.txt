cmake_minimum_required(VERSION 3.5.0)
project(rbggamemanager)

# Build in release by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 11)

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

file(GLOB PLAYING_FILES src/actions/* src/game_components/* src/game_nfa/* src/parser/* src/utils/*)
file(GLOB NETWORKING_FILES src/networking/*)

add_executable(benchmark ${PLAYING_FILES} src/benchmark.cc)
target_link_libraries(benchmark rbgParser)
target_link_libraries(benchmark ${Boost_LIBRARIES})
target_link_libraries(benchmark ${CMAKE_THREAD_LIBS_INIT})

add_executable(host ${PLAYING_FILES} ${NETWORKING_FILES} src/run_server.cc)
target_link_libraries(host rbgParser)
target_link_libraries(host ${Boost_LIBRARIES})
target_link_libraries(host ${CMAKE_THREAD_LIBS_INIT})

add_executable(random_client ${PLAYING_FILES} ${NETWORKING_FILES} src/random_client.cc)
target_link_libraries(random_client rbgParser)
target_link_libraries(random_client ${Boost_LIBRARIES})
target_link_libraries(random_client ${CMAKE_THREAD_LIBS_INIT})

add_executable(material_client ${PLAYING_FILES} ${NETWORKING_FILES} src/material_heuristic_client.cc)
target_link_libraries(material_client rbgParser)
target_link_libraries(material_client ${Boost_LIBRARIES})
target_link_libraries(material_client ${CMAKE_THREAD_LIBS_INIT})

