FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libpqxx-dev

COPY custom/custom_interface.cpp .
COPY custom/query.sql .

RUN g++ -o custom_interface custom_interface.cpp -lpqxx -lpq

ENV TOPIC=db1
ENV USER=user
ENV PASSWORD=password
ENV PORT=5432
ENV DATABASE=db

CMD ./custom_interface --topic=$TOPIC --user=$USER --password=$PASSWORD --port=$PORT --dbname=$DATABASE