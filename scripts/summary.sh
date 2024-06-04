#!/bin/bash
# Create a sumdb, which is a summary of the data in the topic cluster
db="logosdb-sumdb"
network="topic_cluster_topicdb-cluster"
user="user"
pass="password"

interface="logosdb-summary"

# Launch a sumdb postres container
echo "Launching" $db "..."
docker rm -f $db
docker run --name $db -e POSTGRES_USER=$user -e POSTGRES_PASSWORD=$pass -d --network $network postgres:16-alpine3.20

echo "Add transfer medium between topic cluster and sumdb: " $interface "..."
docker build -t logosdb-summary -f src/summary/Dockerfile .

docker rm -f $interface

docker run --network=$network --name $interface --env-file .env -d logosdb-summary

# Output the logs
echo "=============================="
echo "Outputting logs..."
docker logs -f $interface