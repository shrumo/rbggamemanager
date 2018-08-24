#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tChess"
../build/benchmark ../rbgParser/examples/chess.rbg -s 0 -n 1000
../build/benchmark ../rbgParser/examples/chess.rbg -s 0 -d 4
