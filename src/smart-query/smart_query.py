from typing import List, Tuple
from rich import print
from vect_query import get_all_relevant_vectors
from database import SumDB, LogosCluster

'''
This file serves as a smart-query server to communicate with LogosCluster and SumDB.
'''

'''
HELPER FUNCTIONS
'''


def log(message: str, log_file: str = "sumdb_log.txt"):
    """
    Logs a message to the console and a log file.
    """
    print(message)
    with open(log_file, "a") as log_file:
        log_file.write(message + "\n")


'''
MAIN FUNCTION
'''


def smart_query(user_query: str, k=5) -> List[Tuple[float, str, str, int, int, str]]:
    '''
    This is a wrapper of comparing a query against sumDB, then forward valid vectors to LogosCluster to get top-k articles.
    '''
    sumdb = SumDB()
    sumdb.connect()
    log("Fetching all data from the SumDB")
    summarized_chunks = sumdb.get_all()
    log("Data fetched successfully.")

    # Get Relevant Chunks
    threshold = 0.5
    relevant_vectors = get_all_relevant_vectors(
        summarized_chunks, user_query, threshold)

    # Check if relevant vectors are found
    if not relevant_vectors:
        log(f"No relevant vectors found for query '{user_query}'")
        return

    log(f"Found {len(relevant_vectors)} relevant vectors for query '{user_query}'")

    # Get top k articles
    logos_cluster = LogosCluster()
    detail_articles = logos_cluster.find_articles(relevant_vectors, k)

    return detail_articles


def query_server() -> None:
    '''
    This is a uvicorn server to handle query requests.
    '''
    pass


if __name__ == "__main__":
    k = 5
    user_input = r"datamaker is a canadian developer and marketer of test data management software datamaker was founded by mathieu pelletier in 2020"

    print(smart_query(user_input, k))
