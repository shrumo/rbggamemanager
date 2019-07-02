#!/bin/bash

# This script guides user through compiling process on Linux systems.

cd "${0%/*}" # This changes your current directory to the one where the file is placed
FILE=../rbgParser/Makefile
if ! test -f "$FILE"; then
    echo "$FILE does not exist. You should probably call ./pull.sh."
    exit 1
fi
echo "Creating CMakeLists.txt for submodule rbgParser."
cd ../rbgParser/
python3 ../scripts/cmakesubmodule.py > CMakeLists.txt
echo "CMakeLists.txt for submodule created."
cd ../
echo "Creating build directory."
mkdir build -p
cd build
echo "Build directory made at:"
pwd
echo "Using cmake to generate makefiles."
cmake .. -DCMAKE_BUILD_TYPE=Release
echo "Using make to compile the project."
make -j4
echo "Done."
