#!/bin/bash

# This script guides user through visualizaing process on Linux systems.

echo "This requires tkinter, make sure to install it. (sudo apt install python3-tk)" 

${0%/*}/./make.sh
python3 ${0%/*}/../build/visualize_graph.py $1
