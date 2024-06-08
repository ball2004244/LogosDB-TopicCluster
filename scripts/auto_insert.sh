#!/bin/bash
# Insert data dynamically into every node in the cluster
db="logosdb-auto-insert-interface"
network="topic_cluster_topicdb-cluster"

# Build the Docker image
docker build -t auto_insert_interface -f src/auto_insert/Dockerfile .


docker rm -f $db

echo "Auto inserting data into cluster"

# Run the Docker container and pass the db variable
docker run --network=$network --name $db -d auto_insert_interface

# Output the logs
docker logs -f $db