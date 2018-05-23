#!/bin/bash

# This pulls the new version of the code

cd "${0%/*}"
echo "Removing old CMakeLists.txt for submodule rbgParser."
rm ../rbgParser/CMakeLists.txt
echo "Pulling newest commits."
git submodule update --init
cd ../rbgParser
git reset --hard
git checkout master
git pull
cd ../
git checkout master
git pull
echo "Creating CMakeLists.txt for submodule rbgParser."
cd rbgParser/
python3 ../scripts/cmakesubmodule.py > CMakeLists.txt

