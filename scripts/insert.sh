#!/bin/bash
# Insert data into the 1 node cluster
db="logosdb-insert-interface"
network="topic_cluster_topicdb-cluster"

# Build the Docker image
docker build -t insert_interface -f src/insert/Dockerfile .


docker rm -f $db

echo "Inserting data into cluster, at " $db "..."
# Run the Docker container and pass the db variable
docker run --network=$network --name $db -d insert_interface

# Output the logs
docker logs -f $db