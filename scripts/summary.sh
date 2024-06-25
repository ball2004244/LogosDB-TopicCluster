#!/bin/bash
# Create a sumdb, which is a summary of the data in the topic cluster
db="logosdb-sumdb"
network="topic_cluster_topicdb-cluster"
user="user"
pass="password"

interface="logosdb-summary"
volumes="$(pwd)/sumdb"
# Launch a sumdb postres container
echo "Launching" $db "..."
docker run --name $db -e POSTGRES_USER=$user -e POSTGRES_PASSWORD=$pass -e POSTGRES_DB=db -d --network $network --volume $volumes:/var/lib/postgresql/data -p 5432:5432 postgres:16-alpine3.20

# Wait for the PostgreSQL server to be ready
echo "Waiting for PostgreSQL server to be ready..."
for i in {1..10}; do
    if docker exec $db pg_isready; then
        break
    else
        echo "PostgreSQL server not ready, retrying in 5 seconds..."
        sleep 5
    fi
done

echo "Create transfer bridge between TopicCluster & SumDB: " $interface "..."
docker build -t logosdb-summary -f src/summary/Dockerfile .

docker rm -f $interface

#! Uncomment to run on SumAI mode
# docker run --network=$network --name $interface --env-file .env -d logosdb-summary

docker run --network=$network --name $interface -d logosdb-summary

# Output the logs
echo "=============================="
echo "Outputting logs..."
docker logs -f $interface