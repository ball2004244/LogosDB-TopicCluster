FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libpqxx-dev

COPY insert.cpp .
COPY database/ database/
COPY inputs/ inputs/

RUN g++ -o insert insert.cpp database/database.cpp -lpqxx -lpq

CMD ./insert