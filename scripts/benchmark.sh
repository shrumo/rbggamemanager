#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tChess"
../build/rbggamemanager ../rbgParser/examples/chess.rbg -n 10 -s 0
echo -e "\n\tCheckers"
../build/rbggamemanager ../rbgParser/examples/checkers.rbg -n 50 -s 0
echo -e "\n\tReversi"
../build/rbggamemanager ../examples/reversi.rbg -n 50 -s 0
echo -e "\n\tBigReversi"
../build/rbggamemanager ../examples/big_reversi.rbg -n 50 -s 0
echo -e "\n\tBreakthrough"
../build/rbggamemanager ../examples/breakthrough.rbg -n 50 -s 0
echo -e "\n\tAmazons"
../build/rbggamemanager ../examples/amazons.rbg -n 50 -s 0
echo -e "\n\tConnect4"
../build/rbggamemanager ../examples/connect4.rbg -n 50 -s 0
echo -e "\n\tGo"
../build/rbggamemanager ../examples/go.rbg -n 2 -s 0
