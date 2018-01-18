#/bin/bash

./make.sh
echo -e "\n\tChess"
../build/rbggamemanager ../rbgParser/examples/chess.rbg -n 20 -s 0
echo -e "\n\tCheckers"
../build/rbggamemanager ../examples/faster_checkers.rbg -n 50 -s 0
echo -e "\n\tReversi"
../build/rbggamemanager ../examples/reversi.rbg -n 30 -s 0

