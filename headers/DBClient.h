#ifndef DBCLIENT_H
#define DBCLIENT_H

#include <set>
#include <chrono>

#define ENTRY_SIZE 6000
#define NUM_OF_ENTRIES 1000000

class DBClient 
{
    public:
	char* getEntryVal(char startingChar);
	virtual double initializeDB();
	virtual double readEntry(std::string key);
	virtual double insertEntry(std::string key);
	virtual double updateEntry(std::string key);
	virtual double deleteEntry(std::string key);
	virtual double simultaneousReaders(int n, std::string key);
	virtual double simultaneousTasks(int n);
	virtual double performTransactions(int n, double p);

    protected:
	double calculateTime(std::chrono::time_point<std::chrono::high_resolution_clock> start, std::chrono::time_point<std::chrono::high_resolution_clock> end); 
	std::set<int> getRandomKeys(int len, int min, int max);
	
};

#endif
