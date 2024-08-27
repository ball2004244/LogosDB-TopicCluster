from datetime import datetime
from typing import List, Tuple
from psycopg2 import sql
import psycopg2

'''
HELPER FUNCTION
'''


def get_column_names(conn: psycopg2.extensions.connection, table: str) -> List[str]:
    '''
    Fetches and logs the column names of a specified table.
    '''
    query = sql.SQL('''
        SELECT column_name 
        FROM information_schema.columns 
        WHERE table_name = %s
        ORDER BY ordinal_position;
    ''')
    cur = conn.cursor()
    cur.execute(query, (table,))
    columns = cur.fetchall()
    cur.close()
    return [col[0] for col in columns]


'''
MAIN OBJECTS
'''


class SumDB:
    def __init__(self) -> None:
        self.sumdb_topic = 'logosdb-sumdb'  # using localhost for now
        self.port = '5432'
        self.dbname = 'db'  # internal database name
        self.username = 'user'
        self.password = 'password'
        self.table = 'test'  # Name of table to query
        self.conn = None

    def connect(self) -> None:
        self.conn = psycopg2.connect(
            dbname=self.dbname,
            user=self.username,
            password=self.password,
            host=self.sumdb_topic,
            port=self.port
        )

        formatted_datetime = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        print(f'[{formatted_datetime}] Connected to database \'{self.dbname}\' on {self.sumdb_topic}:{self.port} as \'{self.username}\'')

        column_names = self.get_column_names()
        print(
            f"Column names in '{self.table}' table: {', '.join(column_names)}")

        return self.conn

    def __del__(self) -> None:
        if self.conn is not None:
            self.conn.close()
            print('Connection closed.')

    def get_all(self) -> List[Tuple[str]]:
        """
        Fetches all data from the specified table.
        """
        if self.conn is None:
            raise ValueError(
                'Connection to database has not been established. Call connect() first.')

        query = sql.SQL("SELECT chunkstart, chunkend, topic, summary, updatedat FROM {}").format(
            sql.Identifier(self.table))
        cur = self.conn.cursor()
        cur.execute(query)
        data = cur.fetchall()
        cur.close()
        return data


class LogosCluster:
    def __init__(self) -> None:
        self.logos_dbname = 'db'  # internal database name
        self.logos_username = 'user'
        self.logos_password = 'password'
        self.logos_table = 'test'

    def find_articles(self, relevant_vectors: List[Tuple[float, str, int, int, str]], k_docs: int = 5) -> List[Tuple[float, str, str, int, int, str]]:
        '''
        Fetches the detailed articles for the top-k most relevant vectors.
        '''
        output = []

        # Sort relevant vectors by similarity score
        relevant_vectors.sort(key=lambda x: x[0], reverse=True)
        for vect in relevant_vectors[:k_docs]:
            score, row, chunk_start, chunk_end, node_topic = vect
            row_id = int(row.split('.')[0])  # Extract row id from row string

            # Connect to the database first
            logos_conn = psycopg2.connect(
                dbname=self.logos_dbname,
                user=self.logos_username,
                password=self.logos_password,
                host=node_topic,
            )

            # Perform the query on the topic node
            query = sql.SQL('''
                SELECT question, answer, keywords FROM {} WHERE id = %s
            ''').format(sql.Identifier(self.logos_table))

            with logos_conn.cursor() as cur:
                cur.execute(query, (row_id,))
                articles = cur.fetchall()
                output.append(
                    (score, row, articles[0][1], chunk_start, chunk_end, node_topic))

            logos_conn.close()

        return output
