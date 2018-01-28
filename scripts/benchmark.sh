#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tChess (Depth: 0)"
../build/rbggamemanager ../rbgParser/examples/chess.rbg -n 2 -s 0 -d 0
echo -e "\n\tCheckers (Depth: 0)"
../build/rbggamemanager ../rbgParser/examples/checkers.rbg -n 10 -s 0 -d 0
echo -e "\n\tReversi (Depth: 0)"
../build/rbggamemanager ../examples/reversi.rbg -n 10 -s 0 -d 0
echo -e "\n\tBigReversi (Depth: 0)"
../build/rbggamemanager ../examples/big_reversi.rbg -n 10 -s 0 -d 0
echo -e "\n\tBreakthrough (Depth: 0)"
../build/rbggamemanager ../examples/breakthrough.rbg -n 10 -s 0 -d 0
echo -e "\n\tAmazons (Depth: 0)"
../build/rbggamemanager ../examples/amazons.rbg -n 10 -s 0 -d 0
echo -e "\n\tConnect4 (Depth: 0)"
../build/rbggamemanager ../examples/connect4.rbg -n 10 -s 0 -d 0
echo -e "\n\tGo (Depth: 0)"
../build/rbggamemanager ../examples/go.rbg -n 2 -s 0 -d 0
echo -e "\n\tDouble Chess (Depth: 0)"
../build/rbggamemanager ../examples/doublechess.rbg -n 2 -s 0 -d 0
echo -e "\n\tEngdraughts (Depth: 0)"
../build/rbggamemanager ../examples/engdraughts.rbg -n 2 -s 0 -d 0
echo -e "\n\tEngdraughts Split (Depth: 0)"
../build/rbggamemanager ../examples/engdraughts-split.rbg -n 2 -s 0 -d 0
