from multiprocessing import Pool, cpu_count
from typing import List, Tuple
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity

'''
This file implements the logic of the smart query server. It assists query and process all vectors from SumDB
'''


def similarity_search(user_query: str, chunk_summary: str) -> float:
    """
    Calculate the similarity between the user query and a chunk summary using cosine similarity.
    """
    # Create a TF-IDF Vectorizer
    vectorizer = TfidfVectorizer()

    # Combine the user query and chunk summary into a list
    documents = [user_query, chunk_summary]

    # Fit and transform the documents into TF-IDF matrix
    tfidf_matrix = vectorizer.fit_transform(documents)

    # Calculate the cosine similarity between the first and second document
    similarity = cosine_similarity(tfidf_matrix[0:1], tfidf_matrix[1:2])

    # Return the similarity score as a float
    return similarity[0][0]


def process_row(row: str, user_query: str, threshold: float) -> Tuple[str, float]:
    """
    Helper function to process a single row.
    """
    row = row.lower().strip()
    if len(row) <= 5:
        return None

    score = similarity_search(user_query, row)
    if score > threshold:
        return (row, score)
    return None


def get_relevant_vectors_from_summary(chunk_summary: str, user_query: str, threshold: float = 0.5) -> List[Tuple[str, float]]:
    """
    Find all vectors (rows) in the chunk summary that have a similarity score above the threshold with the user query.

    output schema: (row, score)
    """

    rows = chunk_summary.split('\n')

    # Use multiprocessing Pool to parallelize the processing of rows
    with Pool(cpu_count()) as pool:
        results = pool.starmap(process_row, [(row, user_query, threshold) for row in rows])

    # Filter out None results
    relevant_rows = [result for result in results if result is not None]

    return relevant_rows


def get_all_relevant_vectors(raw_data: List[Tuple[str]], user_query: str, threshold: float = 0.5) -> List[Tuple[float, str, int, int, str]]:
    """
    Get all relevant vectors from all chunks in the raw data.
    (which has similarity > threshold)
    
    output schema: (score, row, chunk_start, chunk_end, topic)
    """

    # Do similarity search for each chunk summary
    relevant_vectors = []

    progress = 0  # out of 100%
    # A chunk is considered relevant if 1 row within it is relevant
    for i, chunk_row in enumerate(raw_data):
        if i % (len(raw_data) // 10) == 0 and i != 0:
            progress += 10
            print(f"Progress: {progress}%, Sum Chunk: {i}/{len(raw_data)}")
        chunk_start, chunk_end, topic, chunk_summary = chunk_row[:4]
        valid_rows = get_relevant_vectors_from_summary(
            chunk_summary, user_query, threshold)

        # Add valid vectors from chunk to relevant_vectors
        for row, score in valid_rows:
            relevant_vectors.append(
                (score, row, chunk_start, chunk_end, topic))

    return relevant_vectors
