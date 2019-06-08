#!/bin/bash

# This script guides user through testing process on Linux systems.

./make.sh
python3 ../build/visualize_graph.py $1
