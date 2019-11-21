#!/bin/bash

# This script guides user through process of running checking whether two games are equivalent.

${0%/*}/make.sh

${0%/*}/../build/print $1 >/tmp/game_one --style original_pretty
${0%/*}/../build/print $2 >/tmp/game_two --style original_pretty
DIFF=$(diff /tmp/game_one /tmp/game_two) 
if [ "$DIFF" != "" ] 
then
  diff /tmp/game_one /tmp/game_two
else
  echo "No difference"
fi
