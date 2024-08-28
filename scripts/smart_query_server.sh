network="topic_cluster_topicdb-cluster"
server_name="smart_query_server"
port="8000"

# Build the Docker image
docker build -t smart_query_server -f src/smart_query/Dockerfile .

docker rm -f $server_name

echo "Running the Smart Query Server..."
docker run --network=$network --name $server_name -p $port:8000 -d smart_query_server

# Output the logs
docker logs -f $server_name