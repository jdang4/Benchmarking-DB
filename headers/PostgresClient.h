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
	PostgresClient(std::string host);
	~PostgresClient();
	void connect() override;
	double initializeDB() override;
	double readEntry(bool randomOption) override;
	double insertEntry(int key) override;
	double updateEntry(int key, bool randomOption) override;
	double deleteEntry(int key, bool randomOption) override;
	double simultaneousTasks(bool randomOption) override;
	double performTransactions(int key) override;

    private:
	std::string connection_description;
	char* dataVal;
	char* newVal;

	template<typename Lambda>	
	double run_threads(Lambda f, int begin, bool random, int n = 0);


	

};

#endif
