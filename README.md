# LogosDB-TopicCluster

This repo support creating dynamic docker compose, allow users to manage multiple containers

## Prequisitive

- Docker
- Docker Compose
- C++17

## Usage

### Create cluster

1. To create docker compose dynamically, you need to add the database names you like to `topics.txt`.
   Each database name should be in a new line.

2. Then create the dynamic `compose.yml` file and launch the cluster with

```bash
bash ./scripts/build.sh
```

### Cluster cleaning
- Delete the cluster

```bash
bash ./scripts/clean.sh
```

- Full clear Docker with
```bash
docker system prune -a --volumes
```

### Cluster interaction

You can either insert or query data with the cluster

**1. Insert**

- Enter the target database info
- Bring your input csv file to the `inputs` folder
- Then insert with

```bash
bash ./scripts/insert.sh
```

**2. Query**

- Enter target database info
- Query data from 1 database with

```bash
bash ./scripts/query.sh
```

**3. Custom**
You can also write your own SQL at `custom/query.sql`

Then run custom query with

```bash
bash ./scripts/custom.sh
```

### Dynamic Routing

1. Bring your own data
First put your .csv file in `inputs` folder

Second, modify the process_input.py for convert your data into Cluster readable data

You might need some python libraries for that

```bash
pip install -r requirements.txt
```
Then run process_input script

```bash
python3 process_input.py
```

After all, auto insert processed data with

```bash
bash ./scripts/auto_insert.sh
```

_Note: This process will take time_

2. Generate SumDB
   SumDB acts as a table of content for topic cluster, make querying extremely fast. 
   To create SumDB with default config, you don't need to do anything.

   If you want to use SumDB with intergrated AI, you need to copy `.env.copy` to a new file called `.env` and fill out your Google Gemini API.


Then, run 
```bash 
bash ./scripts/summary.sh
```

which will introduce a summary interface into the TopicCluster, then summarize all data by chunk.

_Note: This process will take time_
