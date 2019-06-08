#!/bin/bash

# This pulls the new version of the code.

cd "${0%/*}"
echo "Removing old CMakeLists.txt for submodule rbgParser."
rm ../rbgParser/CMakeLists.txt -f
echo "Pulling newest commits."
git pull --recurse-submodules

