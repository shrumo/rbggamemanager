cmake_minimum_required(VERSION 3.5.0)
project(rbggamemanager)

# Build in release by default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pipe -Ofast")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -march=native")

add_subdirectory("extern/rbgParser")

find_package(Python3 COMPONENTS Interpreter Development)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/pybind11/.git")
    add_subdirectory(extern/pybind11)
endif()

find_package(Threads)

include_directories(extern/asio/asio/include)

# Include the directories so that finding headers is easier
include_directories(src)
include_directories(.)
include_directories(extern)

set(GRAPH_FILES src/utility/graph.h src/stl_extension/range.h src/stl_extension/argparse.h src/stl_extension/utility.h src/stl_extension/iterator_transform.h)

set(PARSER_FILES src/parser/parser_wrapper.h src/parser/parser_actions.h src/parser/parser_wrapper.cc)

set(GAMESTATE_FILES ${PARSER_FILES} ${GRAPH_FILES} src/game_description/names_resolver.h src/game_description/board.h src/game_state/blocks/block.h
        src/game_description/declarations.h src/game_state/game_state.h src/utility/resetabble_bitarray_stack.h
        src/game_description/moves/arithmetic_operation.cc src/game_description/moves/arithmetic_operation.h
        src/utility/arithmetic_operation_visitor.h
        src/game_description/names_resolver.cc src/game_description/board.cc
        src/utility/calculate_perft.h
        src/game_state/blocks/actions.h src/stl_extension/vector.h
        src/game_state/blocks/modifying_application.h src/game_state/blocks/modifying_application.cc
        src/game_description/construction/graph_creator.cc src/game_description/construction/graph_creator.h
        src/game_description/moves/moves.h
        src/game_description/construction/moves_creator.cc src/game_description/construction/moves_creator.h
        src/game_description/construction/arithmetic_creator.cc src/game_description/construction/arithmetic_creator.h
        src/utility/moves_visitor.h src/utility/printer.cc
        src/utility/move_type.h  src/utility/move_type.cc
        src/utility/printer.h src/game_state/construction/blocks_creator.cc src/game_state/construction/blocks_creator.h
        src/utility/shift_table_checker.h src/utility/shift_table_checker.cc
        src/game_description/moves/arithmetic_operation.cc src/game_description/moves/arithmetic_operation.h
        src/game_description/board.cc src/game_state/blocks/block.h src/game_state/construction/game_state_creator.h
        src/game_state/construction/game_state_creator.cc
        src/utility/illegal_dots.cc src/utility/illegal_dots.h
        )

set(NETWORKING_FILES ${GAMESTATE_FILES} src/networking/server.h src/networking/socket.h src/networking/client.h src/networking/socket.cc)

# Targets requiring python bindings
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/pybind11/.git" AND ${Python3_FOUND})
    pybind11_add_module(rbg_string_graph src/python_bindings/rbg_string_graph.cc ${GAMESTATE_FILES})
    target_link_libraries(rbg_string_graph PRIVATE rbgParser)

    add_custom_target(visualize_graph ALL
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/python_widgets/graph_widget.py ${CMAKE_CURRENT_BINARY_DIR}/graph_widget.py
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/python_widgets/visualize_graph.py ${CMAKE_CURRENT_BINARY_DIR}/visualize_graph.py
            DEPENDS rbg_string_graph)

    pybind11_add_module(rbg src/python_bindings/rbg.cc ${GAMESTATE_FILES} ${CONSTRUCTION_FILES} ${NETWORKING_FILES})
  add_custom_target(start_random_python_client ALL
              COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/apps/start_random_python_client.py ${CMAKE_CURRENT_BINARY_DIR}/start_random_python_client.py
              DEPENDS rbg)
    target_link_libraries(rbg PRIVATE rbgParser)
else()
    message("For python binding install python3 development package. (sudo apt install python3-dev)")
endif ()


add_executable(benchmark ${GRAPH_FILES} ${PARSER_FILES}
        ${CONSTRUCTION_FILES} ${GAMESTATE_FILES} apps/benchmark.cc)

target_link_libraries(benchmark rbgParser)

add_executable(explain ${GRAPH_FILES} ${PARSER_FILES}
        ${CONSTRUCTION_FILES} ${GAMESTATE_FILES} ${NETWORKING_FILES} apps/explain.cc)
target_compile_options(explain PRIVATE "-g" "-O0")

target_link_libraries(explain rbgParser)
target_link_libraries(explain pthread)


add_executable(move_decoder ${GRAPH_FILES} ${PARSER_FILES}
        ${CONSTRUCTION_FILES} ${GAMESTATE_FILES} ${NETWORKING_FILES} apps/move_decoder.cc)

target_link_libraries(move_decoder rbgParser)
target_link_libraries(move_decoder pthread)

add_executable(add_dots ${GRAPH_FILES} ${PARSER_FILES}
        ${CONSTRUCTION_FILES} ${GAMESTATE_FILES} apps/add_dots.cc)
target_link_libraries(add_dots rbgParser)
target_link_libraries(add_dots pthread)

add_executable(mark_illegal_dots ${GRAPH_FILES} ${PARSER_FILES} ${CONSTRUCTION_FILES} ${GAMESTATE_FILES} apps/mark_illegal_dots.cc)
target_link_libraries(mark_illegal_dots rbgParser)
target_link_libraries(mark_illegal_dots pthread)

add_executable(start_server ${NETWORKING_FILES} apps/start_server.cc)
target_link_libraries(start_server rbgParser)
target_link_libraries(start_server pthread)

add_executable(start_random_client ${NETWORKING_FILES} apps/start_random_client.cc)
target_link_libraries(start_random_client rbgParser)
target_link_libraries(start_random_client pthread)

# There is a bug in CMake, so that in order to run tests you have to build executables manually
enable_testing()
add_executable(graph_test ${GRAPH_FILES} tests/graph_test.cc)
add_test(graph_test graph_test)

add_executable(parser_test ${GRAPH_FILES} ${PARSER_FILES} tests/parser_test.cc)
target_link_libraries(parser_test rbgParser)
add_test(parser_test parser_test)

add_executable(nfa_board_product_test ${GAMESTATE_FILES} tests/nfa_board_product_test.cc)
target_link_libraries(nfa_board_product_test rbgParser)
add_test(nfa_board_product_test nfa_board_product_test)

add_executable(creator_test ${GAMESTATE_FILES} tests/creator_test.cc)
target_link_libraries(creator_test rbgParser)
add_test(creator_test creator_test)

add_executable(single_blocks_test ${GAMESTATE_FILES} tests/single_blocks_test.cc)
target_link_libraries(single_blocks_test rbgParser)
add_test(single_blocks_test single_blocks_test)

add_executable(declarations_test ${GAMESTATE_FILES} tests/declarations_test.cc)
target_link_libraries(declarations_test rbgParser)
add_test(declarations_test declarations_test)

add_executable(namesresolver_test src/game_description/names_resolver.h src/game_description/names_resolver.cc tests/namesresolver_test.cc)
add_test(namesresolver_test namesresolver_test)

add_executable(gamestate_simple_test ${GAMESTATE_FILES} tests/game_state_simple_test.cc)
target_link_libraries(gamestate_simple_test rbgParser)
add_test(gamestate_simple_test gamestate_simple_test)

add_executable(amazons_test ${GAMESTATE_FILES} tests/amazons_test.cc)
target_link_libraries(amazons_test rbgParser)
add_test(amazons_test amazons_test)

add_executable(chess_test ${GAMESTATE_FILES} tests/chess_test.cc)
target_link_libraries(chess_test rbgParser)
add_test(chess_test chess_test)
target_compile_options(chess_test PRIVATE "-g" "-O0")

add_executable(arimaa_test ${GAMESTATE_FILES} tests/arimaa_test.cc)
target_link_libraries(arimaa_test rbgParser)
add_test(arimaa_test arimaa_test)

add_executable(hex_test ${GAMESTATE_FILES} tests/hex_test.cc)
target_link_libraries(hex_test rbgParser)
add_test(hex_test hex_test)

add_executable(reversi_test ${GAMESTATE_FILES} tests/reversi_test.cc)
target_link_libraries(reversi_test rbgParser)
add_test(reversi_test reversi_test)

add_executable(tictactoe_test ${GAMESTATE_FILES} tests/tictactoe_test.cc)
target_link_libraries(tictactoe_test rbgParser)
add_test(tictactoe_test tictactoe_test)

add_executable(weird_conditions_test ${GAMESTATE_FILES} tests/weird_conditions_test.cc)
target_link_libraries(weird_conditions_test rbgParser)
add_test(weird_conditions_test weird_conditions_test)

add_executable(server_encoding_test ${GAMESTATE_FILES} ${NETWORKING_FILES} tests/server_encoding_test.cc)
target_link_libraries(server_encoding_test rbgParser)
add_test(server_encoding_test server_encoding_test)
target_link_libraries(server_encoding_test pthread)

add_executable(board_test src/game_description/board.h src/game_description/board.cc tests/board_test.cc)
add_test(board_test board_test)

# Tests requiring the python bindings
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/pybind11/.git" AND ${Python3_FOUND})
    add_custom_target(string_graph_python_bindings_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/string_graph_python_bindings_test.py ${CMAKE_CURRENT_BINARY_DIR}/string_graph_python_bindings_test.py
            DEPENDS rbg_string_graph)
    add_test(string_graph_python_bindings_test ${Python3_EXECUTABLE} string_graph_python_bindings_test.py)
    add_custom_target(python_bindings_perft_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/python_bindings_perft_test.py ${CMAKE_CURRENT_BINARY_DIR}/python_bindings_perft_test.py
            DEPENDS rbg)
    add_test(python_bindings_perft_test ${Python3_EXECUTABLE} python_bindings_perft_test.py)
    add_custom_target(python_bindings_server_communication_test ALL 
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/python_bindings_server_communication_test.py ${CMAKE_CURRENT_BINARY_DIR}/python_bindings_server_communication_test.py
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/apps/start_random_python_client.py ${CMAKE_CURRENT_BINARY_DIR}/start_random_python_client.py)
    add_test(python_bindings_server_communication_test ${Python3_EXECUTABLE} python_bindings_server_communication_test.py)
endif ()

# Tests requiring the python interpreter
if (${Python3_Interpreter_FOUND})
    add_custom_target(server_communication_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/server_communication_test.py ${CMAKE_CURRENT_BINARY_DIR}/server_communication_test.py)
    add_test(server_communication_test ${Python3_EXECUTABLE} server_communication_test.py)

    add_custom_target(gess_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/gess_test.py ${CMAKE_CURRENT_BINARY_DIR}/gess_test.py)
    add_test(gess_test ${Python3_EXECUTABLE} gess_test.py)

    add_custom_target(exceeds_deadline_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/exceeds_deadline_test.py ${CMAKE_CURRENT_BINARY_DIR}/exceeds_deadline_test.py)
    add_test(exceeds_deadline_test ${Python3_EXECUTABLE} exceeds_deadline_test.py)

    add_custom_target(manager_exceeds_deadline_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/manager_exceeds_deadline_test.py ${CMAKE_CURRENT_BINARY_DIR}/manager_exceeds_deadline_test.py)
    add_test(manager_exceeds_deadline_test ${Python3_EXECUTABLE} manager_exceeds_deadline_test.py)

    add_custom_target(server_logging_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/server_logging_test.py ${CMAKE_CURRENT_BINARY_DIR}/server_logging_test.py)
    add_test(server_logging_test ${Python3_EXECUTABLE} server_logging_test.py)

    add_custom_target(continous_play_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/continous_play_test.py ${CMAKE_CURRENT_BINARY_DIR}/continous_play_test.py)
    add_test(continous_play_test ${Python3_EXECUTABLE} continous_play_test.py)

    add_custom_target(client_disconnect_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/client_disconnect_test.py ${CMAKE_CURRENT_BINARY_DIR}/client_disconnect_test.py)
    add_test(client_disconnect_test ${Python3_EXECUTABLE} client_disconnect_test.py)

    add_custom_target(server_games_limit_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/server_games_limit_test.py ${CMAKE_CURRENT_BINARY_DIR}/server_games_limit_test.py)
    add_test(server_games_limit_test ${Python3_EXECUTABLE} server_games_limit_test.py)

    add_custom_target(moves_logging_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/moves_logging_test.py ${CMAKE_CURRENT_BINARY_DIR}/moves_logging_test.py)
    add_test(moves_logging_test ${Python3_EXECUTABLE} moves_logging_test.py)

    add_custom_target(player_long_compilation_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/player_long_compilation_test.py ${CMAKE_CURRENT_BINARY_DIR}/player_long_compilation_test.py)
    add_test(player_long_compilation_test ${Python3_EXECUTABLE} player_long_compilation_test.py)

    add_custom_target(add_dots_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/add_dots_test.py ${CMAKE_CURRENT_BINARY_DIR}/add_dots_test.py)
    add_test(add_dots_test ${Python3_EXECUTABLE} add_dots_test.py)

    add_custom_target(mark_illegal_dots_test ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tests/mark_illegal_dots_test.py ${CMAKE_CURRENT_BINARY_DIR}/mark_illegal_dots_test.py)
    add_test(mark_illegal_dots_test ${Python3_EXECUTABLE} mark_illegal_dots_test.py)
endif ()
