#!/bin/bash

# This pulls the new version of the code
rm ../rbgParser/CMakeLists.txt
git submodule update --init
git checkout master
git pull
