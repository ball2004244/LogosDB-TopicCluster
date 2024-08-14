#! Approach 1: AI-chunking summary, currently deprecated
# FROM gcc:latest

# WORKDIR /app

# RUN apt-get update && apt-get install -y libpqxx-dev libcurl4-openssl-dev libjsoncpp-dev

# RUN ln -s /usr/include/jsoncpp/json/ /usr/include/json

# COPY src/summary/ .
# COPY src/database/ database/
# COPY inputs/topics.txt inputs/topics.txt

# RUN g++ -I/usr/include/pqxx -I/usr/include/curl -ljsoncpp -lpqxx -lpq -lcurl -o summary summary.cpp ai_summary.cpp database/database.cpp 

# CMD ./summary

#! Approach 2: statistical summary with keyword extraction from every data
# FROM gcc:latest

# WORKDIR /app

# RUN apt-get update && apt-get install -y libpqxx-dev

# COPY src/summary/ .
# COPY src/database/ database/
# COPY inputs/topics.txt inputs/topics.txt

# RUN g++ -o summary summary.cpp keyword_counter.cpp database/database.cpp -lpqxx -lpq

# CMD ./summary

#! Approach 3: Extractive summary with sentence ranking using Cython & Multiprocessing
# TODO: Implement this approach

# Use gcc as the base image
FROM gcc:latest

WORKDIR /app

# Install Python 3.11 and other necessary dependencies
RUN apt-get update && apt-get install -y \
    python3.11 \
    python3.11-dev \
    python3.11-venv \
    python3-pip \
    libpqxx-dev

# Set Python 3.11 as the default python3
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.11 1

# Create and activate a virtual environment
RUN python3 -m venv /app/venv
ENV PATH="/app/venv/bin:$PATH"

# Install Python packages within the virtual environment
RUN pip install --upgrade pip && \
    pip install nltk networkx numpy scikit-learn

# Copy source files
COPY src/summary/ . 
COPY src/database/ database/
COPY inputs/topics.txt inputs/topics.txt

# Compile the C++ code
RUN g++ -o summary summary.cpp extract_summary.cpp database/database.cpp -lpqxx -lpq -I/usr/include/python3.11 -lpython3.11

# Use JSON array for CMD to prevent unintended behavior related to OS signals
CMD ./summary