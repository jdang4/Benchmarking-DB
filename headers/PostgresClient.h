#ifndef POSTGRESCLIENT_H
#define POSTGRESCLIENT_H

#include "DBClient.h"
#include <pqxx/pqxx>
#include <set>
#include <chrono>

using namespace pqxx;

class DBClient;

class PostgresClient : public DBClient
{
    public:
	PostgresClient(std::string host, int n, int threads);
	~PostgresClient();
	void connect() override;
	void disconnect() override;
	double initializeDB() override;
	double readEntry(std::string key, bool randomOption) override;
	double insertEntry(std::string key) override;
	double updateEntry(std::string key, bool randomOption) override;
	double deleteEntry(std::string key, bool randomOption) override;
	double simultaneousTasks(int n, bool randomOption) override;
	double performTransactions(int n, double p, bool randomOption) override;

    private:
	//connection* postgres;
	std::string connection_description;
	char* dataVal;
	char* newVal;
	int numOfRuns;
	int numOfThreads;


	template<typename Lambda>	
	double run_threads(Lambda f, int begin, bool random, int n = 0);

};

#endif
