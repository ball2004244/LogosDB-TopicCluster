#!/bin/bash
# Set init variables
db="logosdb-debug-interface"
network="topic_cluster_topicdb-cluster"

# Nav to desired directory
cd debug

# Build the Docker image
docker build -t debug_interface .


docker rm -f $db

# Run the Docker container and pass the db variable
docker run --network=$network --name $db -d debug_interface
cd ..