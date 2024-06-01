#! /bin/bash
cd src/topic_cluster
docker compose down
cd ../..

docker stop logosdb-insert-interface
docker rm -f logosdb-insert-interface

docker stop logosdb-debug-interface
docker rm -f logosdb-debug-interface
