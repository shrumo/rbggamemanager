#/bin/bash

cd "${0%/*}"
./make.sh
# echo -e "\n\tChess"
# ../build/benchmark ../rbgParser/examples/chess.rbg -n 2 -s 0
# ../build/benchmark ../rbgParser/examples/chess.rbg -s 0 -d 3
# echo -e "\n\tChess Capture"
# ../build/benchmark ../rbgParser/examples/chess-cap.rbg -n 10 -s 0
# ../build/benchmark ../rbgParser/examples/chess-cap.rbg -s 0 -d 3
# echo -e "\n\tCheckers"
# ../build/benchmark ../rbgParser/examples/checkers.rbg -n 10 -s 0 
# ../build/benchmark ../rbgParser/examples/checkers.rbg -s 0 -d 3
echo -e "\n\tReversi"
../build/benchmark ../rbgParser/examples/reversi.rbg -n 10 -s 0
../build/benchmark ../rbgParser/examples/reversi.rbg -s 0 -d 3
# echo -e "\n\tBigReversi"
# ../build/benchmark ../examples/big_reversi.rbg -n 10 -s 0 
# ../build/benchmark ../examples/big_reversi.rbg -s 0 -d 3
echo -e "\n\tBreakthrough"
../build/benchmark ../rbgParser/examples/breakthrough.rbg -n 10 -s 0
../build/benchmark ../rbgParser/examples/breakthrough.rbg -s 0 -d 3
echo -e "\n\tAmazons"
../build/benchmark ../rbgParser/examples/amazons.rbg -n 10 -s 0
../build/benchmark ../rbgParser/examples/amazons.rbg -s 0 -d 2
echo -e "\n\tConnect4"
../build/benchmark ../rbgParser/examples/connect4.rbg -n 10 -s 0
../build/benchmark ../rbgParser/examples/connect4.rbg -s 0 -d 3
echo -e "\n\tTictactoe"
../build/benchmark ../rbgParser/examples/tictactoe.rbg -n 10 -s 0
../build/benchmark ../rbgParser/examples/tictactoe.rbg -s 0 -d 3
# echo -e "\n\tGo"
# ../build/benchmark ../rbgParser/examples/go.rbg -n 2 -s 0
# ../build/benchmark ../rbgParser/examples/go.rbg -s 0 -d 2
# echo -e "\n\tDouble Chess"
# ../build/benchmark ../rbgParser/examples/doublechess.rbg -n 2 -s 0
# ../build/benchmark ../rbgParser/examples/doublechess.rbg -s 0 -d 3
# echo -e "\n\tEngdraughts"
# ../build/benchmark ../rbgParser/examples/engdraughts.rbg -n 2 -s 0
# ../build/benchmark ../rbgParser/examples/engdraughts.rbg -s 0 -d 3
# echo -e "\n\tEngdraughts Split"
# ../build/benchmark ../rbgParser/examples/engdraughts-split.rbg -n 2 -s 0
#../build/benchmark ../rbgParser/examples/engdraughts-split.rbg -s 0 -d 3
