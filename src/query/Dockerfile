FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libpqxx-dev

COPY src/query/query.cpp query.cpp
COPY src/database/ database/
COPY inputs/ inputs/

RUN g++ -o query query.cpp database/database.cpp -lpqxx -lpq

CMD ./query