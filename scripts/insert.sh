#!/bin/bash
# Insert data into the 1 node cluster
db="logosdb-insert-interface"
network="topic_cluster_topicdb-cluster"

# Copy inputs dir into cur dir
cp -r inputs/ src/
cd src/

# Build the Docker image
docker build -t insert_interface .

docker rm -f $db

# Run the Docker container and pass the db variable
docker run --network=$network --name $db -d insert_interface

rm -rf inputs
cd ../