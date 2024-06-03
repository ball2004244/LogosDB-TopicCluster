# LogosDB-TopicCluster

This repo support creating dynamic docker compose, allow users to manage multiple containers

## Prequisitive
- Docker
- Docker Compose
- C++11

## Usage
### Create cluster
1. To create docker compose dynamically, you need to add the database names you like to `topics.txt`.
Each database name should be in a new line.

2. Then create the dynamic `compose.yml` file and launch the cluster with

```bash
./script/build.sh
```

3. Full delete the cluster
```bash
./script/clean.sh
```

### Cluster interaction
You can either insert or query data with the cluster

**1. Insert**
- Enter the target database info
- Bring your input csv file to the `inputs` folder
- Then insert with
```bash
./script/insert.sh
```

**2. Query**
- Enter target database info
- Query data from 1 database with
```bash
./script/query.sh
```

- Or you can query the whole cluster with
```bash
./script/query_all.sh
```

**3. Custom**
You can also write your own SQL at `debug/query.sql`

Then run custom query with
```bash
./script/debug.sh
```