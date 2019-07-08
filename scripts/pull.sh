#!/bin/bash

# This pulls the new version of the code.

cd "${0%/*}" # This changes your current directory to the one where the file is placed
echo "Removing old CMakeLists.txt for submodule rbgParser."
rm ../rbgParser/CMakeLists.txt -f
echo "Pulling newest commits."
git submodule init
git submodule update
git pull --recurse-submodules

