#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tAmazons"
../build/benchmark ../rbgParser/examples/amazons.rbg -s 0 -n 2000
../build/benchmark ../rbgParser/examples/amazons.rbg -d 2

echo -e "\n\tBreakthrough"
../build/benchmark ../rbgParser/examples/breakthrough.rbg -n 100000 -s 0
../build/benchmark ../rbgParser/examples/breakthrough.rbg -d 5

echo -e "\n\tChess-cap"
../build/benchmark ../rbgParser/examples/chess-cap.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/chess-cap.rbg  -d 5

echo -e "\n\tChess"
../build/benchmark ../rbgParser/examples/chess-opt.rbg -n 1000 -s 0
../build/benchmark ../rbgParser/examples/chess-opt.rbg -d 5

echo -e "\n\tChineseCheckers6"
../build/benchmark ../rbgParser/examples/chineseCheckers6.rbg -n 1000 -s 0
../build/benchmark ../rbgParser/examples/chineseCheckers6.rbg -d 4

echo -e "\n\tConnect4"
../build/benchmark ../rbgParser/examples/connect4.rbg -n 100000 -s 0
../build/benchmark ../rbgParser/examples/connect4.rbg -d 8

echo -e "\n\tDoubleChess"
../build/benchmark ../rbgParser/examples/doubleChess.rbg -n 100 -s 0
../build/benchmark ../rbgParser/examples/doubleChess.rbg -d 3

echo -e "\n\tEnglishCheckers-split"
../build/benchmark ../rbgParser/examples/englishCheckers-split.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/englishCheckers-split.rbg -d 8

echo -e "\n\tEnglishCheckers"
../build/benchmark ../rbgParser/examples/englishCheckers.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/englishCheckers.rbg -d 8

echo -e "\n\tGo"
../build/benchmark ../rbgParser/examples/go.rbg -n 200 -s 0
../build/benchmark ../rbgParser/examples/go.rbg -d 2

echo -e "\n\tGomoku"
../build/benchmark ../rbgParser/examples/gomoku.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/gomoku.rbg -d 3

echo -e "\n\tHex"
../build/benchmark ../rbgParser/examples/hex.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/hex.rbg -d 3

echo -e "\n\tInternationalCheckers"
../build/benchmark ../rbgParser/examples/internationalCheckers.rbg -n 1000 -s 0
../build/benchmark ../rbgParser/examples/internationalCheckers.rbg -d 6

echo -e "\n\tPaperSoccer"
../build/benchmark ../rbgParser/examples/paperSoccer.rbg -n 100 -s 0
../build/benchmark ../rbgParser/examples/paperSoccer.rbg -d 5

echo -e "\n\tReversi"
../build/benchmark ../rbgParser/examples/reversi.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/reversi.rbg -d 8

echo -e "\n\tTheMillGame"
../build/benchmark ../rbgParser/examples/theMillGame.rbg -n 10000 -s 0
../build/benchmark ../rbgParser/examples/theMillGame.rbg -d 5

echo -e "\n\tTicTacToe"
../build/benchmark ../rbgParser/examples/ticTacToe.rbg -n 1000000 -s 0
../build/benchmark ../rbgParser/examples/ticTacToe.rbg -d 10
