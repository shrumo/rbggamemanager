#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tChess (Depth: 1)"
../build/rbggamemanager ../rbgParser/examples/chess.rbg -n 2 -s 0 -d 1
echo -e "\n\tCheckers (Depth: 3)"
../build/rbggamemanager ../rbgParser/examples/checkers.rbg -n 10 -s 0 -d 3
echo -e "\n\tReversi (Depth: 3)"
../build/rbggamemanager ../examples/reversi.rbg -n 10 -s 0 -d 3
echo -e "\n\tBigReversi (Depth: 2)"
../build/rbggamemanager ../examples/big_reversi.rbg -n 10 -s 0 -d 2
echo -e "\n\tBreakthrough (Depth: 3)"
../build/rbggamemanager ../examples/breakthrough.rbg -n 10 -s 0 -d 3
echo -e "\n\tAmazons (Depth: 2)"
../build/rbggamemanager ../examples/amazons.rbg -n 10 -s 0 -d 1
echo -e "\n\tConnect4 (Depth: 3)"
../build/rbggamemanager ../examples/connect4.rbg -n 10 -s 0 -d 3
echo -e "\n\tGo (Depth: 0)"
../build/rbggamemanager ../examples/go.rbg -n 2 -s 0 -d 0
