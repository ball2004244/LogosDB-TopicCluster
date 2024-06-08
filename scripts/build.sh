#!/bin/bash

# this file is used to build the dynamic docker compose and run it
cd src/topic_cluster
g++ build_compose.cpp -o build_compose

echo "Building the dynamic docker compose"
./build_compose

echo "Creating the docker compose file based on inputs/topic.txt"
docker compose up -d
cd ../..
