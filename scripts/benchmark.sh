#!/bin/bash

# This script guides user through benchmarking process on Linux systems.

cd "${0%/*}" # This changes your current directory to the one where the file is placed
./make.sh

echo "Chess"
../build/benchmark ../extern/rbgGames/games/chess.rbg --depth 4
echo "Reversi"
../build/benchmark ../extern/rbgGames/games/reversi.rbg --depth 4
echo "Amazons"
../build/benchmark ../extern/rbgGames/games/amazons.rbg --depth 2
echo "TicTacToe"
../build/benchmark ../extern/rbgGames/games/ticTacToe.rbg --depth 9
