#ifndef DBCLIENT_H
#define DBCLIENT_H

#include <vector>
#include <chrono>

#define ENTRY_SIZE 6000
#define NUM_OF_ENTRIES 1000000

class DBClient 
{
    public:
	char* getEntryVal(char startingChar);
	virtual void connect();
	virtual void disconnect();
	virtual double initializeDB();
	virtual double readEntry(std::string key, bool randomOption);
	virtual double insertEntry(std::string key);
	virtual double updateEntry(std::string key, bool randomOption);
	virtual double deleteEntry(std::string key, bool randomOption);
	virtual double simultaneousReaders(int n, std::string key);
	virtual double simultaneousTasks(int n, bool randomOption);
	virtual double performTransactions(int n, double p, bool randomOption);
	std::vector<int> getRandomKeys(int len, int min, int max);

    protected:
	double calculateTime(std::chrono::time_point<std::chrono::high_resolution_clock> start, std::chrono::time_point<std::chrono::high_resolution_clock> end); 
	
};

#endif
