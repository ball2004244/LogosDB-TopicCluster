#!/bin/bash

# Build the Docker image
docker build -t interface .

docker rm -f logosdb-interface

# Run the Docker container
docker run --network=logosdb-topiccluster_topicdb-cluster --name logosdb-interface -d interface