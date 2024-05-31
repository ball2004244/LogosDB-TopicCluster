#!/bin/bash

# Build the Docker image
docker build -t interface .

# Run the Docker container
docker run --rm --network=logosdb-topiccluster_topicdb-cluster --name logosdb-interface -d interface