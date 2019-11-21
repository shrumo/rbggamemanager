#!/bin/bash

# This script guides user through testing process on Linux systems.

cd "${0%/*}" # This changes your current directory to the one where the file is placed
./make.sh
cd ../build/
if [[ $# -eq 0 ]]
  then
    make test
  else
    for name in $1 $1_test 
    do
      make $name 2> /dev/null
      if [[ -f ./$name ]]
      then
        ./$name
      elif [[ -f ./$name.py ]]
      then
        python3 ./$name.py
      fi
    done
fi
