#!/bin/bash
# Query all data from the 1 node cluster
db="logosdb-query-interface"
network="topic_cluster_topicdb-cluster"

# Build the Docker image
docker build -t query_interface -f src/query/Dockerfile .


docker rm -f $db

echo "Querying data from 1 cluster's node, at" $db "..."
docker run --network=$network --name $db -d query_interface

# Output the logs
docker logs -f $db

