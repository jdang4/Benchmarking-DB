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
	RedisClient();
	~RedisClient();
	void connect() override;
	double initializeDB() override;
	double readEntry(std::string key, bool randomOption) override;
	double insertEntry(std::string key) override;
	double updateEntry(std::string key, bool randomOption) override;
	double deleteEntry(std::string key, bool randomOption) override;
	double simultaneousTasks(int n, bool randomOption) override;
	double performTransactions(int n, double p, bool randomOption) override;
	 
    private:
	sw::redis::Redis* redis;
	char* dataVal;
	char* newVal;
	
	template<typename Lambda>	
	double run_threads(Lambda f, int begin, bool random, int n = 0);

};

#endif
