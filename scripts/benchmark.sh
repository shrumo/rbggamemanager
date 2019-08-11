#!/bin/bash

# This script guides user through benchmarking process on Linux systems.

cd "${0%/*}" # This changes your current directory to the one where the file is placed
./make.sh

echo "Chess"
../build/benchmark ../rbgParser/examples/chess.rbg 4
echo "Reversi"
../build/benchmark ../rbgParser/examples/reversi.rbg 4
echo "Amazons"
../build/benchmark ../rbgParser/examples/amazons.rbg 2
echo "TicTacToe"
../build/benchmark ../rbgParser/examples/ticTacToe.rbg 9
