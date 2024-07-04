echo "Building test-inference container"
docker build -t test-inference .

echo "Running test-inference container"
docker run -it --rm test-inference
