FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y \
	libcurl4-openssl-dev \
	libpq-dev \
	libjsoncpp-dev \
	libpqxx-dev

RUN ln -s /usr/include/jsoncpp/json/ /usr/include/json

COPY src/auto_insert/ .
COPY src/database/ database/
COPY inputs/topics.txt topics.txt
COPY inputs/input.csv input.csv

RUN g++ -o auto_insert auto_insert.cpp database/database.cpp kw_extract.cpp -lpqxx -lpq -lcurl -ljsoncpp

CMD ./auto_insert