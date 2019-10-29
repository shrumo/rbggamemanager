#!/bin/bash

# This script guides user through process of running a server, two clients and making them play for some time.

${0%/*}/make.sh

echo "Starting server"
${0%/*}/../build/start_server $1 5024 --log $3 &
echo "Waiting for server to turn up"
sleep 5
echo "Starting first client"
${0%/*}/../build/start_random_client localhost 5024 --time $2 > /dev/null &
echo "Done."
echo "Starting second client and playing for" $2 "seconds"
${0%/*}/../build/start_random_client localhost 5024 --time $2 > /dev/null
echo "Done. Results written to" $3 
