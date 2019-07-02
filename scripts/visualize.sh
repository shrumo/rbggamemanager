#!/bin/bash

# This script guides user through visualizaing process on Linux systems.

${0%/*}/./make.sh
python3 ${0%/*}/../build/visualize_graph.py $1
