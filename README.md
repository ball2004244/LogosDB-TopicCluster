# LogosDB-TopicCluster

This repo support creating dynamic docker compose, allow users to manage multiple containers

## Prequisitive
- Docker
- Docker Compose


## Usage

1. To create docker compose dynamically, you need to add the database names you like to `topics.txt`.
Each database name should be in a new line.

2. Then create the dynamic `compose.yml` file and launch the cluster with

```bash
./script/build.sh
```

3. Interact with specific database within cluster via
```bash
./script/interface.sh
```

4. Terminate the cluster with
```bash
./script/clean.sh
```