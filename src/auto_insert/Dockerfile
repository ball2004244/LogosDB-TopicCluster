FROM gcc:latest

WORKDIR /app

# Install dependencies required for CPR
RUN apt-get update && apt-get install -y \
	cmake \
	git \
	libcurl4-openssl-dev \
    libpq-dev \
    libcurl4-openssl-dev \
    libjsoncpp-dev \
    libpqxx-dev

RUN ln -s /usr/include/jsoncpp/json/ /usr/include/json

# Clone and build CPR from source
RUN git clone https://github.com/libcpr/cpr.git && \
	cd cpr && \
	mkdir build && \
	cd build && \
	cmake .. && \
	make && \
	make install

# Update linker cache
RUN ldconfig

COPY src/auto_insert/ .
COPY src/database/ database/
COPY inputs/ inputs/

RUN g++ -o auto_insert auto_insert.cpp database/database.cpp kw_extract.cpp -lpqxx -lpq -lcurl -ljsoncpp -lcpr

CMD ./auto_insert