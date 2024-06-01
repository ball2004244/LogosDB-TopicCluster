FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libpqxx-dev

COPY debug_interface.cpp .
COPY query.sql .

RUN g++ -o debug_interface debug_interface.cpp -lpqxx -lpq

ENV TOPIC=db1
ENV USER=user
ENV PASSWORD=password
ENV PORT=5432
ENV DATABASE=db

CMD ./debug_interface --topic=$TOPIC --user=$USER --password=$PASSWORD --port=$PORT --dbname=$DATABASE