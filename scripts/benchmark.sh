#/bin/bash

cd "${0%/*}"
./make.sh
echo -e "\n\tChess"
../build/benchmark ../rbgParser/examples/chess.rbg -n 2 -s 0
../build/benchmark ../rbgParser/examples/chess.rbg -s 0 -d 4
# echo -e "\n\tPaper Soccer"
# ../build/benchmark ../rbgParser/examples/paperSoccer.rbg -n 2 -s 0
# ../build/benchmark ../rbgParser/examples/paperSoccer.rbg -s 0 -d 2
 echo -e "\n\tEnglish Checkers"
 ../build/benchmark ../rbgParser/examples/englishCheckers.rbg -n 10 -s 0 
 ../build/benchmark ../rbgParser/examples/englishCheckers.rbg -s 0 -d 3
 echo -e "\n\tReversi"
 ../build/benchmark ../rbgParser/examples/reversi.rbg -n 10 -s 0
 ../build/benchmark ../rbgParser/examples/reversi.rbg -s 0 -d 3
 echo -e "\n\tBreakthrough"
 ../build/benchmark ../rbgParser/examples/breakthrough.rbg -n 10 -s 0
 ../build/benchmark ../rbgParser/examples/breakthrough.rbg -s 0 -d 2
 echo -e "\n\tAmazons"
 ../build/benchmark ../rbgParser/examples/amazons.rbg -n 10 -s 0
 ../build/benchmark ../rbgParser/examples/amazons.rbg -s 0 -d 2
 echo -e "\n\tConnect4"
 ../build/benchmark ../rbgParser/examples/connect4.rbg -n 10 -s 0
 ../build/benchmark ../rbgParser/examples/connect4.rbg -s 0 -d 3
 echo -e "\n\tTictactoe"
 ../build/benchmark ../rbgParser/examples/ticTacToe.rbg -n 10 -s 0
 ../build/benchmark ../rbgParser/examples/ticTacToe.rbg -s 0 -d 3
 echo -e "\n\tChinese Checkers"
 ../build/benchmark ../rbgParser/examples/chineseCheckers6.rbg -n 10 -s 0 
 ../build/benchmark ../rbgParser/examples/chineseCheckers6.rbg -s 0 -d 3
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
