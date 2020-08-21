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

	void selectDB(int db, std::string host, int recordSize);
	void setThreads_and_Entries(int threads, int entries);
	void performRandomization();
	void initializeDB();
	void connect();
	void disconnect();
	void openCSV(int n, int recordSize);
	void closeCSV();
	double getReadOutput(int threads, bool csvOption, bool random);
	double getInsertOutput(int threads, int start, bool csvOption);
	double getUpdateOutput(int threads, int start, bool csvOption, bool random);
	double getDeleteOutput(int threads, int start, bool csvOption, bool random);
	double getSimultaneousTasksOutput(int threads, bool csvOption, bool random);
	double getTransactionsOutput(int threads, int start, bool csvOption);
	
    private:
	DBClient* client;
	int trials;
	int dbClient;
	std::string dbName;
	bool showOutputs;
	bool randomOption;
	std::ofstream* csv;
};


#endif
