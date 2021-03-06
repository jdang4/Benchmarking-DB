FROM ubuntu:latest
FROM gcc:4.9


RUN apt-get update && apt-get install -y cmake apt-utils

RUN apt-get install -y postgresql postgresql-contrib libpqxx-4.0 libpq-dev libpqxx-dev

WORKDIR /benchmark

COPY . /benchmark

# adding the C++ db clients installation

WORKDIR /benchmark/include

RUN git clone https://github.com/redis/hiredis.git

WORKDIR /benchmark/include/hiredis

RUN make

RUN make install

WORKDIR /benchmark/include

RUN git clone https://github.com/sewenew/redis-plus-plus.git

WORKDIR /benchmark/include/redis-plus-plus/compile

RUN cmake -DCMAKE_BUILD_TYPE=Release -REDIS_PLUS_PLUS_BUILD_TEST=OFF ..

RUN make

RUN make install

WORKDIR /benchmark/include/libpqxx-4.0

CMD ["./configure"]

RUN make

# compiling the benchmark app

WORKDIR /benchmark/

RUN make

CMD ["./benchmark"]
