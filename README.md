# LogosDB-TopicCluster

This repo support creating dynamic docker compose, allow users to manage multiple containers

## Set up

1. Compile the code

```bash
g++ build_compose.cpp -o build
```

2. Add the database names you like to `input.txt`

3. Then generate the dynamic `compose.yml` file

```bash
./build
```

4. Launch the database cluster with

```bash
docker compose up -d
```
