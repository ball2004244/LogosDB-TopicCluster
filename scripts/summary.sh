#!/bin/bash
# Create a sumdb, which is a summary of the data in the topic cluster
db="logosdb-sumdb"
network="topic_cluster_topicdb-cluster"
user="user"
pass="password"

interface="logosdb-summary"
volumes="../sumdb"
# Launch a sumdb postres container
echo "Launching" $db "..."
docker run --name $db -e POSTGRES_USER=$user -e POSTGRES_PASSWORD=$pass -e POSTGRES_DB=db -d --network $network postgres:16-alpine3.20 --volume $volumes:/var/lib/postgresql/data

echo "Create transfer bridge between TopicCluster & SumDB: " $interface "..."
docker build -t logosdb-summary -f src/summary/Dockerfile .

docker rm -f $interface

docker run --network=$network --name $interface --env-file .env -d logosdb-summary

# Output the logs
echo "=============================="
echo "Outputting logs..."
docker logs -f $interface