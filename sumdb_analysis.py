from datetime import datetime
from collections import defaultdict
from psycopg2 import sql
from rich import print
import psycopg2
import sys

def log(message: str) -> None:
    """
    Logs a message to the console.
    """
    # print(message)
    with open("sumdb_log.txt", "a") as log_file:
        log_file.write(message + "\n")

def get_column_names(conn, table):
    """
    Fetches and logs the column names of a specified table.
    """
    query = sql.SQL("""
        SELECT column_name 
        FROM information_schema.columns 
        WHERE table_name = %s
        ORDER BY ordinal_position;
    """)
    cur = conn.cursor()
    cur.execute(query, (table,))
    columns = cur.fetchall()
    cur.close()
    return [col[0] for col in columns]

def main():
    try:
        filename = "inputs/input.csv"
        topicFileName = "inputs/topics.txt"

        # find len
        with open(topicFileName, 'r') as file:
            true_topics = file.readlines()
            true_topics = [topic.strip() for topic in true_topics]

        topic = "localhost"  # using localhost for now
        port = "5432"
        dbname = "db"  # internal database name
        username = "user"
        password = "password"

        table = "test"  # Name of table to query

        # Connect to the database
        conn = psycopg2.connect(
            dbname=dbname,
            user=username,
            password=password,
            host=topic,
            port=port
        )

        # Format datetime for readability
        formatted_datetime = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log(f"[{formatted_datetime}] Connected to database '{dbname}' on {topic}:{port} as '{username}'")
        # Get and log column names
        column_names = get_column_names(conn, table)
        log("Column names in '{}' table: {}".format(table, ", ".join(column_names)))

        # Prepare and execute the query
        query = sql.SQL("SELECT chunkstart, chunkend, topic FROM {}").format(sql.Identifier(table))
        with conn.cursor() as cur:
            cur.execute(query)
            rows = cur.fetchall()

        conn.close()

        topic_chunk_count = defaultdict(int)
        row_topic_pairs = defaultdict(list)
        row_topic_count = defaultdict(int)

        for row in rows:
            chunk_start, chunk_end, topic = row

            topic_chunk_count[topic] += 1
            row_topic_pairs[topic].append((chunk_start, chunk_end))
            row_topic_count[topic] += abs(chunk_end - chunk_start) + 1

        # log the count of chunks for each topic
        log('\nTOPIC NODE ANALYSIS')
        for topic, count in topic_chunk_count.items():
            log(f"{topic}: {count} chunks")

        log('Chunk Count: ' + str(len(rows)) + ' saved chunks')
        log(f'Actual topic node: {len(topic_chunk_count)} nodes')
        log(f'Expected topic node: {len(true_topics)} nodes')
        log(f'Missing {len(true_topics) - len(topic_chunk_count)} nodes: {set(true_topics) - set(topic_chunk_count.keys())}')
        
        log('\nDATA COUNT ANALYSIS')
        for topic, rows in row_topic_pairs.items():
            log(f"{topic}: {rows} (Rows: {row_topic_count[topic]})")

        total_rows = sum(row_topic_count.values())
        log(f'Total rows: {total_rows}')

        log('\nCONTINUITY CHECK')
        for topic, chunks in row_topic_pairs.items():
            # Sort the chunks by chunkstart to ensure correct order
            chunks.sort(key=lambda x: x[0])
            for i in range(len(chunks) - 1):
                current_chunk_end = chunks[i][1]
                next_chunk_start = chunks[i + 1][0]
                # Check if there is a gap
                if next_chunk_start != current_chunk_end + 1:
                    log(f"Gap found in topic '{topic}' within chunks [{current_chunk_end}, {next_chunk_start}]")

    except Exception as e:
        log(str(e), file=sys.stderr)

#! Get wrong chunkstart & chunkend from SumDB
#! Maybe issues with summary C++ algorithm or with this script
#TODO: Check if this script is working correctly, otherwise check the summary algorithm
if __name__ == "__main__":
    main()