#ifndef POSTGRESCLIENT_H
#define POSTGRESCLIENT_H

#include "DBClient.h"
#include <pqxx/pqxx>
#include <set>
#include <chrono>

class DBClient;

class PostgresClient : public DBClient
{
    public:
	PostgresClient(std::string host);
	~PostgresClient();
	/*
	double initializeDB() override;
	double readEntry(std::string key) override;
	double insertEntry(std::string key) override;
	double updateEntry(std::string key, std::string newVal) override;
	double simultaneousTasks(int n) override;
	double performTransactins(int n, double p) override;
	*/

    private:
	pqxx::connection* postgres;
	char* entryVal;
};

#endif
