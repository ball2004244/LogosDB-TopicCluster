#! /bin/bash

echo "Terminate summary interface"
docker stop logosdb-summary
docker rm -f logosdb-summary

echo "Terminate auto insert interface"
docker stop logosdb-auto-insert
docker rm -f logosdb-auto-insert

echo "Stopping and removing all other interfaces"
docker stop logosdb-insert-interface
docker rm -f logosdb-insert-interface

docker stop logosdb-query-interface
docker rm -f logosdb-query-interface

docker stop logosdb-custom-interface
docker rm -f logosdb-custom-interface

echo "Terminate sumdb"
docker stop logosdb-sumdb
docker rm -f logosdb-sumdb

cd src/topic_cluster
echo "Terminate all topic cluster nodes"
docker compose down
cd ../..

echo "Delete network"
docker network rm topic_cluster_topicdb-cluster

echo "Successfully cleaned up all interfaces and topic cluster nodes"