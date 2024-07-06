network="topic_cluster_topicdb-cluster"
name="kw-extract-server"

echo "Building kw_extract server container"
docker build -t kw_extract_server -f ai_models/Dockerfile .

docker rm -f $name

echo "Running kw_extract server container"
docker run --network=$network --name $name -d kw_extract_server
