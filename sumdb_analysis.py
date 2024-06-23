import psycopg2
from psycopg2 import sql
import sys

def get_column_names(conn, table):
    """
    Fetches and prints the column names of a specified table.
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

        # Get and print column names
        column_names = get_column_names(conn, table)
        print("Column names in '{}' table: {}".format(table, ", ".join(column_names)))

        # Prepare and execute the query
        query = sql.SQL("SELECT chunkstart, chunkend, topic FROM {}").format(sql.Identifier(table))
        cur = conn.cursor()
        cur.execute(query)

        # Fetch and print all rows
        rows = cur.fetchall()
        # Initialize a dictionary to count chunks per topic
        topic_chunk_count = {}

        # Iterate over each row and count chunks per topic
        for row in rows:
            topic = row[2]  # Assuming the topic is in the third column
            if topic in topic_chunk_count:
                topic_chunk_count[topic] += 1
            else:
                topic_chunk_count[topic] = 1

        # Print the count of chunks for each topic
        for topic, count in topic_chunk_count.items():
            print(f"{topic}: {count} chunks")

        print('Chunk Count: ' + str(len(rows)) + ' saved chunks')
        print('Num topic node:', len(topic_chunk_count), 'nodes')
        # Close the cursor and connection
        cur.close()
        conn.close()

        return 0
    except Exception as e:
        print(e, file=sys.stderr)
        return 1

if __name__ == "__main__":
    sys.exit(main())