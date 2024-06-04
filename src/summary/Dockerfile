FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libpqxx-dev libcurl4-openssl-dev libjsoncpp-dev

RUN ln -s /usr/include/jsoncpp/json/ /usr/include/json

COPY src/summary/summary.cpp summary.cpp
COPY src/database/ database/
COPY inputs/ inputs/

RUN g++ -I/usr/include/pqxx -I/usr/include/curl -ljsoncpp -lpqxx -lpq -lcurl -o summary summary.cpp database/database.cpp

CMD ./summary