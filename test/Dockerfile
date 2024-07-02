# FROM gcc:latest
FROM python:3.11

WORKDIR /app

# RUN apt-get update && apt-get install -y libpqxx-dev

# Copy everything from the current directory to /app in the image
COPY . .

# RUN g++ -o test test.cpp database/database.cpp -lpqxx -lpq -std=c++17

# CMD ./test

RUN pip install psycopg2-binary

CMD ["python3", "test.py"]