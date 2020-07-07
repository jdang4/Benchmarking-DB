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
	void connect() override;
	void disconnect() override;
	double initializeDB() override;
	double readEntry(std::string key) override;
	double insertEntry(std::string key) override;
	double updateEntry(std::string key) override;
	double deleteEntry(std::string key) override;
	double simultaneousReaders(int n, std::string key) override;
	double simultaneousTasks(int n) override;
	double performTransactions(int n, double p) override;

    private:
	pqxx::connection* postgres;
	std::string host;
	char* dataVal;
	char* newDataVal;
};

#endif
