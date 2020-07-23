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
	double getReadOutput();
	double getInsertOutput();
	double getUpdateOutput();
	double getDeleteOutput();
	double getSimultaneousTasksOutput(int n, int num = 0);
	double getTransactionsOutput(int n, double p, int num = 0);
	
    private:
	DBClient* client;
	int trials;
	int dbClient;
	bool showOutputs;
	std::ofstream* csv;
};


#endif
