#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tChess"
../build/benchmark ../rbgParser/examples/chess-opt.rbg -n 1000 -s 0
../build/benchmark ../rbgParser/examples/chess-opt.rbg -s 0 -d 4
