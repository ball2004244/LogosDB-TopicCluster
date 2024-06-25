#!/bin/bash
network="topic_cluster_topicdb-cluster"
debug="debug"

docker pull jupyter/base-notebook
docker run -it --network=$network --name $debug --rm -p 8888:8888 -v $(pwd):/home/jovyan/work jupyter/base-notebook 