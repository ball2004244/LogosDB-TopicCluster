#!/bin/bash

# this file is used to build the dynamic docker compose and run it
g++ build_compose.cpp -o build_compose

./build_compose

docker-compose up -d