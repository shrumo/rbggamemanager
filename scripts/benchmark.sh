#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tChess"
../build/rbggamemanager ../examples/chess.rbg -n 2 -s 0
../build/rbggamemanager ../examples/chess.rbg -s 0 -d 3
echo -e "\n\tChess Capture"
../build/rbggamemanager ../examples/chess-cap.rbg -n 10 -s 0
../build/rbggamemanager ../examples/chess-cap.rbg -s 0 -d 3
echo -e "\n\tCheckers"
../build/rbggamemanager ../rbgParser/examples/checkers.rbg -n 10 -s 0 
../build/rbggamemanager ../rbgParser/examples/checkers.rbg -s 0 -d 3
echo -e "\n\tReversi"
../build/rbggamemanager ../examples/reversi.rbg -n 10 -s 0 
../build/rbggamemanager ../examples/reversi.rbg -s 0 -d 3
echo -e "\n\tBigReversi"
../build/rbggamemanager ../examples/big_reversi.rbg -n 10 -s 0 
../build/rbggamemanager ../examples/big_reversi.rbg -s 0 -d 3
echo -e "\n\tBreakthrough"
../build/rbggamemanager ../examples/breakthrough.rbg -n 10 -s 0 
../build/rbggamemanager ../examples/breakthrough.rbg -s 0 -d 3
echo -e "\n\tAmazons"
../build/rbggamemanager ../examples/amazons.rbg -n 10 -s 0 
../build/rbggamemanager ../examples/amazons.rbg -s 0 -d 2
echo -e "\n\tConnect4"
../build/rbggamemanager ../examples/connect4.rbg -n 10 -s 0 
../build/rbggamemanager ../examples/connect4.rbg -s 0 -d 3
echo -e "\n\tGo"
../build/rbggamemanager ../examples/go.rbg -n 2 -s 0 
../build/rbggamemanager ../examples/go.rbg -s 0 -d 2 
echo -e "\n\tDouble Chess"
../build/rbggamemanager ../examples/doublechess.rbg -n 2 -s 0 
../build/rbggamemanager ../examples/doublechess.rbg -s 0 -d 3
echo -e "\n\tEngdraughts"
../build/rbggamemanager ../examples/engdraughts.rbg -n 2 -s 0 
../build/rbggamemanager ../examples/engdraughts.rbg -s 0 -d 3
echo -e "\n\tEngdraughts Split"
../build/rbggamemanager ../examples/engdraughts-split.rbg -n 2 -s 0 
../build/rbggamemanager ../examples/engdraughts-split.rbg -s 0 -d 3
