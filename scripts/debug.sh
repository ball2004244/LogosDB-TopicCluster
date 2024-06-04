#!/bin/bash
# Set init variables
db="logosdb-debug-interface"
network="topic_cluster_topicdb-cluster"


# Build the Docker image
docker build -t debug_interface -f debug/Dockerfile .


docker rm -f $db

echo "[Debugging Mode] cluster's node: " $db "..."
# Run the Docker container and pass the db variable
docker run --network=$network --name $db -d debug_interface

# Get output logs
docker logs -f $db