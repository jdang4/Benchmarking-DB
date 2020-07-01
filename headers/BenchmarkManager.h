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
	void openCSV();
	void closeCSV();
	void getReadOutput();
	void getInsertOutput();
	void getUpdateOutput();
	void getDeleteOutput();
	void getSimultaneousReadersOutput(int n);
	void getSimultaneousTasksOutput(int n);
	void getTransactionsOutput(int n, double p);
	
    private:
	DBClient* client;
	int trials;
	std::ofstream* csv;
};

#endif
