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
	BenchmarkManager(int n, bool option);
	~BenchmarkManager();

	void selectDB(int db, std::string host);
	void setThreads_and_Runs(int threads, int runs);
	void initializeDB();
	void connect();
	void disconnect();
	void openCSV(int n);
	void closeCSV();
	double getReadOutput(int threads);
	double getInsertOutput(int threads, int start);
	double getUpdateOutput(int threads, int start);
	double getDeleteOutput(int threads, int start);
	double getSimultaneousTasksOutput(int threads, int num = 0);
	double getTransactionsOutput(int threads, int num = 0);
	
    private:
	DBClient* client;
	int trials;
	int dbClient;
	bool showOutputs;
	std::ofstream* csv;
};


#endif
