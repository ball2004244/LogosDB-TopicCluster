from collections import defaultdict
from datetime import datetime
from psycopg2 import sql
import psycopg2
import json

topic_file = 'topics.txt'
topics = []

with open(topic_file, 'r') as f:
    for line in f:
        topics.append(line.strip())

username = 'user'
password = 'password'
dbname = 'db'  # Fixed variable name from 'db' to 'dbname'
port = 5432
sumdb = 'logosdb-sumdb'

table = 'test'
CHUNK_SIZE = 10000
# each topic will be a host within cluster

# Connect to logosdb-sumdb first
sumdb_conn = psycopg2.connect(
    dbname=dbname,
    user=username,
    password=password,
    host=sumdb
)

for topic in topics:
    conn = psycopg2.connect(
        dbname=dbname,
        user=username,
        password=password,
        host=topic
        )


    offset = 0
    while True:
        print(f'Insert to SumDB for topic {topic}')

        # Modified query to include LIMIT and OFFSET for chunk processing
        query = sql.SQL("SELECT id, keywords FROM {} ORDER BY id LIMIT %s OFFSET %s").format(sql.Identifier(table))        

        with conn.cursor() as cur:
            cur.execute(query, (CHUNK_SIZE, offset))
            rows = cur.fetchall()
            
            if not rows:
                break  # Exit loop if no more rows are fetched
            
            offset += CHUNK_SIZE  # Increase offset for the next chunk

        # get first and last id
        first_id = rows[0][0]
        last_id = rows[-1][0]
        # summary is a dict of keyword map to list of rowIds
        
        #build summary here
        summary = defaultdict(list)
        
        # extract keywords col, which is the 2nd col
        for (rowId, keywords) in rows:
            for keyword in keywords:
                summary[keyword].append(rowId)

        query = sql.SQL("CREATE TABLE IF NOT EXISTS {} (id INT PRIMARY KEY, chunkstart INT, chunkend INT, topic TEXT, summary TEXT, updatedat TIMESTAMP)").format(sql.Identifier(table))
        with sumdb_conn.cursor() as cur:
            cur.execute(query)
            sumdb_conn.commit()

        # get max id, if not then set to 1
        max_id = 1
        query = sql.SQL("SELECT MAX(id) FROM {}").format(sql.Identifier(table))
        with sumdb_conn.cursor() as cur:
            cur.execute(query)
            max_id = cur.fetchone()[0]
            if max_id is None:
                max_id = 1
            else:
                max_id += 1

        # convert datetime to postgres acceptable type
        current = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        # now build SumDBRow to push to SumDB
        sumDBRow = (max_id, first_id, last_id, topic, json.dumps(summary), current)

        # now insert into sumdb
        query = sql.SQL("INSERT INTO {} (id, chunkstart, chunkend, topic, summary, updatedat) VALUES (%s, %s, %s, %s, %s, %s)").format(sql.Identifier(table))
        with sumdb_conn.cursor() as cur:
            cur.execute(query, sumDBRow)
            sumdb_conn.commit()    

    conn.close()
sumdb_conn.close()
print('Done')