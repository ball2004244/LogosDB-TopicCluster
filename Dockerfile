FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libpqxx-dev

COPY . .

RUN g++ -o interface interface.cpp -lpqxx -lpq

CMD ["./interface"]