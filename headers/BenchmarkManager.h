#ifndef BENCHMARKMANAGER_H
#define BENCHMARKMANAGER_H

#include <fstream>
#include <iostream>
#include "RedisClient.h"
#include "PostgresClient.h"


class DBClient;

class BenchmarkManager 
{
    public:
	BenchmarkManager(int n);
	~BenchmarkManager();

	void selectDB(int db, std::string host);
	void initializeDB();
	void connect();
	void disconnect();
	void openCSV();
	void closeCSV();
	void getReadOutput(int num = 0);
	void getInsertOutput(int num = 0);
	void getUpdateOutput(int num = 0);
	void getDeleteOutput(int num = 0);
	void getSimultaneousReadersOutput(int n, int num = 0);
	void getSimultaneousTasksOutput(int n, int num = 0);
	void getTransactionsOutput(int n, double p, int num = 0);
	
    private:
	DBClient* client;
	int trials;
	int dbClient;
	std::ofstream* csv;
};


#endif
