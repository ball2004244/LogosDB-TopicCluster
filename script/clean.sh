#! /bin/bash

docker compose down

docker stop logosdb-interface

docker rm logosdb-interface