#!/bin/bash

# This script guides user through testing process on Linux systems.

cd "${0%/*}"
./make.sh
cd ../build/
make test
