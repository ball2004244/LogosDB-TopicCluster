#! /bin/bash
cd compose
docker compose down
cd ..

docker stop insert_interface
docker rm -f insert_interface

docker stop debug_interface
docker rm -f debug_interface