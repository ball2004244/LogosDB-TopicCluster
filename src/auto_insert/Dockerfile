FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libpqxx-dev

COPY src/auto_insert/auto_insert.cpp auto_insert.cpp
COPY src/database/ database/
COPY inputs/ inputs/

RUN g++ -o auto_insert auto_insert.cpp database/database.cpp -lpqxx -lpq

CMD ./auto_insert