#ifndef REDISCLIENT_H
#define REDISCLIENT_H

#include "DBClient.h"
#include "../include/redis-plus-plus/src/sw/redis++/redis++.h"
#include <set>
#include <chrono>

class DBClient;

class RedisClient : public DBClient
{
    public:
	RedisClient(std::string host_name, int record_size);
	~RedisClient();
	void connect() override;
	double initializeDB() override;
	double readEntry(bool randomOption) override;
	double insertEntry(int key) override;
	double updateEntry(int key, bool randomOption) override;
	double deleteEntry(int key, bool randomOption) override;
	double simultaneousTasks(bool randomOption) override;
	double performTransactions(int key) override;
	 
    private:
	char* dataVal;
	char* newVal;
	
	sw::redis::ConnectionOptions options;
	sw::redis::ConnectionPoolOptions pool_options;

	
	template<typename Lambda>	
	double run_threads(Lambda f, int begin, bool random, int n = 0);

};

#endif
