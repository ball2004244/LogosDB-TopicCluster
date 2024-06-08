#!/bin/bash
# Set init variables
db="logosdb-custom-interface"
network="topic_cluster_topicdb-cluster"


# Build the Docker image
docker build -t custom_interface -f custom/Dockerfile .


docker rm -f $db

echo "[Custom Mode] cluster's node: " $db "..."
# Run the Docker container and pass the db variable
docker run --network=$network --name $db -d custom_interface

# Get output logs
docker logs -f $db
