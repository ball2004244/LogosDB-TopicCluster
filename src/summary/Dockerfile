#! This is for the AI version of the summary, currently deprecated
# FROM gcc:latest

# WORKDIR /app

# RUN apt-get update && apt-get install -y libpqxx-dev libcurl4-openssl-dev libjsoncpp-dev

# RUN ln -s /usr/include/jsoncpp/json/ /usr/include/json

# COPY src/summary/ .
# COPY src/database/ database/
# COPY inputs/topics.txt inputs/topics.txt

# RUN g++ -I/usr/include/pqxx -I/usr/include/curl -ljsoncpp -lpqxx -lpq -lcurl -o summary summary.cpp ai_summary.cpp database/database.cpp 

# CMD ./summary

#! This is a statistical approach to the summary
FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libpqxx-dev

COPY src/summary/ .
COPY src/database/ database/
COPY inputs/topics.txt inputs/topics.txt

# RUN g++ -o auto_insert auto_insert.cpp database/database.cpp -lpqxx -lpq
RUN g++ -o summary summary.cpp keyword_counter.cpp database/database.cpp -lpqxx -lpq

CMD ./summary