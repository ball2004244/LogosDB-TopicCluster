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


def get_relevant_vectors_from_summary(chunk_summary: str, user_query: str, threshold: float = 0.5) -> List[Tuple[str, float]]:
    """
    Find all vectors (rows) in the chunk summary that have a similarity score above the threshold with the user query.
    """

    relevant_rows = []
    current_row = ""

    # Iterate over each character in the chunk summary
    for char in chunk_summary:
        # Split summary into rows by newline symbol
        if char != '\n':
            current_row += char
            continue

        # Skip rows that are too short
        current_row = current_row.lower().strip()
        if len(current_row) <= 5:
            current_row = ""
            continue

        score = similarity_search(user_query, current_row)

        # only add rows with a similarity score above the threshold
        if score > threshold:
            relevant_rows.append((current_row, score))

        current_row = ""

    # Check the last row if it doesn't end with a newline
    current_row = current_row.lower().strip()
    if len(current_row) > 5:
        score = similarity_search(user_query, current_row)
        if score > threshold:
            relevant_rows.append((current_row, score))

    return relevant_rows


def get_all_relevant_vectors(raw_data: List[Tuple[str]], user_query: str, threshold: float = 0.5) -> List[Tuple[float, str, int, int, str]]:
    """
    Get all relevant vectors from all chunks in the raw data.
    (which has similarity > threshold)
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
