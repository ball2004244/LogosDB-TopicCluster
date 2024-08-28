from typing import List, Tuple, Dict, Union
# from rich import print
from fastapi import FastAPI
from pydantic import BaseModel
from database import SumDB, LogosCluster
from vect_query import get_all_relevant_vectors
import time
import uvicorn
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
MAIN LOGIC FUNCTION
'''


def smart_query(user_query: str, k_docs: int = 5, threshold: float = 0.5) -> List[Tuple[float, str, str, int, int, str]]:
    '''
    This is a wrapper of comparing a query against sumDB, then forward valid vectors to LogosCluster to get top-k articles.

    output schema: (similarity, query, article, chunkstart, chunkend, topic)
    '''

    sumdb = SumDB()

    log("Fetching all data from the SumDB")
    summarized_chunks = sumdb.get_all()
    log("Data fetched successfully.")

    # Get Relevant Chunks higher than threshold
    relevant_vectors = get_all_relevant_vectors(
        summarized_chunks, user_query, threshold)

    # Check if relevant vectors are found
    if not relevant_vectors:
        log(f"No relevant vectors found for query '{user_query}'")
        return

    log(f"Found {len(relevant_vectors)} relevant vectors for query '{user_query}'")

    # Get top k articles
    logos_cluster = LogosCluster()
    detail_articles = logos_cluster.find_articles(relevant_vectors, k_docs)

    return detail_articles


'''
SETUP QUERY SERVER
'''


class SmartQueryRequest(BaseModel):
    query: str
    k: int = 5


def query_server() -> None:
    '''
    This is a uvicorn server to handle query requests.
    '''
    # HOST = "localhost" # run in development
    HOST = "0.0.0.0" # run in production
    PORT = 8000

    app = FastAPI()

    @app.get("/smart_query")
    def smart_query_endpoint(request: SmartQueryRequest) -> Dict[str, List[Dict[str, Union[float, str]]]]:
        '''
        This is a FastAPI endpoint to handle smart query requests.
        '''
        start = time.perf_counter()
        json_output = {
            "results": []
        }
    
        # get query and k from request
        query = request.query
        k = request.k

        query_results = smart_query(query, k)
    
        # reformat as json
        if query_results is None or not query_results:
            return json_output
    
        for result in query_results:
            json_output["results"].append({
                "score": result[0],
                "query": result[1],
                "article": result[2],
                "topic": result[5]
            })

        print(f"Query took {time.perf_counter() - start} seconds ~ {time.perf_counter() - start / 60} minutes")
        return json_output

    log(f"Starting server at {HOST}:{PORT}")
    uvicorn.run(app, host=HOST, port=PORT)


if __name__ == "__main__":
    # k = 5
    # user_input = r"datamaker is a canadian developer and marketer of test data management software datamaker was founded by mathieu pelletier in 2020"

    # print(smart_query(user_input, k))

    query_server()
