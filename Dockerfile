FROM ubuntu:latest
FROM gcc:4.9
#ENV PATH="/jonathand/:${PATH}"


RUN apt-get update && apt-get install -y cmake apt-utils

RUN apt-get install -y postgresql postgresql-contrib libpqxx-4.0 libpq-dev libpqxx-dev

WORKDIR /redis-client

COPY . /redis-client

WORKDIR /redis-client/include/hiredis

RUN make

RUN make install

WORKDIR /redis-client/include/redis-plus-plus/compile

RUN cmake -DCMAKE_BUILD_TYPE=Release -REDIS_PLUS_PLUS_BUILD_TEST=OFF ..

RUN make

RUN make install

WORKDIR /redis-client/include/libpqxx-4.0

CMD ["./configure"]

RUN make

WORKDIR /redis-client/

RUN make

CMD ["./benchmark"]

#WORKDIR /etc/postgresql/9.4/main 

#RUN ls
