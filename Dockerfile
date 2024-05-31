FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libpqxx-dev

COPY interface.cpp .
COPY query.sql .

RUN g++ -o interface interface.cpp -lpqxx -lpq

ENV TOPIC=db1
ENV USER=user
ENV PASSWORD=password
ENV PORT=5432
ENV DATABASE=db

CMD ./interface --topic=$TOPIC --user=$USER --password=$PASSWORD --port=$PORT --dbname=$DATABASE