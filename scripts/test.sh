#!/bin/bash

# This script guides user through testing process on Linux systems.

cd "${0%/*}"
./make.sh
cd ../build/
if [[ $# -eq 0 ]]
  then
    make test
  else
    make $1
    if [[ -f ./$1 ]]
    then
        ./$1
    elif [[ -f ./$1.py ]]
    then
        python3 ./$1.py
    fi
fi
