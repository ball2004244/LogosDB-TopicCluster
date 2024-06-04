#! /bin/bash

echo "Terminate summary interface"
docker stop logosdb-summary
docker rm -f logosdb-summary

echo "Stopping and removing all other interfaces"
docker stop logosdb-insert-interface
docker rm -f logosdb-insert-interface

docker stop logosdb-query-interface
docker rm -f logosdb-query-interface

docker stop logosdb-debug-interface
docker rm -f logosdb-debug-interface

echo "Terminate sumdb"
docker stop logosdb-sumdb
docker rm -f logosdb-sumdb

cd src/topic_cluster
echo "Terminate all topic cluster nodes"
docker compose down
cd ../..

echo "Delete network"
docker network rm topic_cluster_topicdb-cluster
