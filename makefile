CFLAGS = -g -std=c++11 -Wall

MYPATH = include/redis-plus-plus/compile/lib/libredis++.a include/hiredis/libhiredis.a

all: benchmark

benchmark: benchmark.o BenchmarkManager.o DBClient.o RedisClient.o PostgresClient.o
	g++ $(CFLAGS) benchmark.o BenchmarkManager.o RedisClient.o DBClient.o PostgresClient.o -o benchmark $(MYPATH) -pthread -lpqxx -lpq

benchmark.o: benchmark.cpp $(MYPATH)
	g++ $(CFLAGS) -c benchmark.cpp -pthread -lpqxx -lpq

BenchmarkManager.o: BenchmarkManager.cpp headers/BenchmarkManager.h headers/RedisClient.h headers/PostgresClient.h $(MYPATH)
	g++ $(CFLAGS) -c BenchmarkManager.cpp -pthread -lpqxx -lpq

DBClient.o: DBClient.cpp headers/DBClient.h
	g++ $(CFLAGS) -c DBClient.cpp

RedisClient.o: RedisClient.cpp headers/RedisClient.h headers/DBClient.h $(MYPATH)
	g++ $(CFLAGS) -c RedisClient.cpp -pthread

PostgresClient.o: PostgresClient.cpp headers/PostgresClient.h headers/PostgresClient.h 
	g++ $(CFLAGS) -c PostgresClient.cpp -pthread -lpqxx -lpq

clean:
	rm -f benchmark.o BenchmarkManager.o DBClient.o RedisClient.o PostgresClient.o benchmark
