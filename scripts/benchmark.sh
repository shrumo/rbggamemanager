#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tChess"
../build/benchmark ../examples/chess.rbg -n 2 -s 0
../build/benchmark ../examples/chess.rbg -s 0 -d 3
# echo -e "\n\tChess Capture"
# ../build/benchmark ../examples/chess-cap.rbg -n 10 -s 0
# ../build/benchmark ../examples/chess-cap.rbg -s 0 -d 3
# echo -e "\n\tCheckers"
# ../build/benchmark ../rbgParser/examples/checkers.rbg -n 10 -s 0 
# ../build/benchmark ../rbgParser/examples/checkers.rbg -s 0 -d 3
echo -e "\n\tReversi"
../build/benchmark ../examples/reversi.rbg -n 10 -s 0 
../build/benchmark ../examples/reversi.rbg -s 0 -d 3
# echo -e "\n\tBigReversi"
# ../build/benchmark ../examples/big_reversi.rbg -n 10 -s 0 
# ../build/benchmark ../examples/big_reversi.rbg -s 0 -d 3
echo -e "\n\tBreakthrough"
../build/benchmark ../examples/breakthrough.rbg -n 10 -s 0 
../build/benchmark ../examples/breakthrough.rbg -s 0 -d 3
echo -e "\n\tAmazons"
../build/benchmark ../examples/amazons.rbg -n 10 -s 0 
../build/benchmark ../examples/amazons.rbg -s 0 -d 2
echo -e "\n\tConnect4"
../build/benchmark ../examples/connect4.rbg -n 10 -s 0 
../build/benchmark ../examples/connect4.rbg -s 0 -d 3
echo -e "\n\tTictactoe"
../build/benchmark ../examples/tictactoe.rbg -n 10 -s 0 
../build/benchmark ../examples/tictactoe.rbg -s 0 -d 3
# echo -e "\n\tGo"
# ../build/benchmark ../examples/go.rbg -n 2 -s 0 
# ../build/benchmark ../examples/go.rbg -s 0 -d 2 
# echo -e "\n\tDouble Chess"
# ../build/benchmark ../examples/doublechess.rbg -n 2 -s 0 
# ../build/benchmark ../examples/doublechess.rbg -s 0 -d 3
# echo -e "\n\tEngdraughts"
# ../build/benchmark ../examples/engdraughts.rbg -n 2 -s 0 
# ../build/benchmark ../examples/engdraughts.rbg -s 0 -d 3
# echo -e "\n\tEngdraughts Split"
# ../build/benchmark ../examples/engdraughts-split.rbg -n 2 -s 0 
#../build/benchmark ../examples/engdraughts-split.rbg -s 0 -d 3
