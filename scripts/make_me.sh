#!/bin/bash

# This script guides user through compiling process. It worked for me.

echo "Creating CMakeLists.txt for submodule rbgParser."
cd ../rbgParser/
python3 ../scripts/create_cmakelists.py > CMakeLists.txt
echo "CMakeLists.txt for submodule created."
cd ../
echo "Creating build directory."
mkdir build -p
cd build
echo "Build directory made at:"
pwd
echo "Using cmake to generate makefiles."
cmake ..
echo "Using make to compile the project."
make -j4
echo "Done."
