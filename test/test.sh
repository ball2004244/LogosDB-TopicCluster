#!/bin/bash
db="logosdb-test-module"
network="topic_cluster_topicdb-cluster"

# Build the Docker image in the current directory
docker build -t test-module . --no-cache

# Remove any existing containers with the same name
docker rm -f $db

echo "Running test module"

# Run the Docker container and pass the db variable
docker run --network=$network --name $db -d test-module

# Output the logs
echo "Outputting logs..."
docker logs -f $db

echo "Finished Testing"