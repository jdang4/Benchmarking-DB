#ifndef DBCLIENT_H
#define DBCLIENT_H

#include <vector>
#include <chrono>

#define NUM_OF_ENTRIES 1000000

class DBClient 
{
    public:
	char* getEntryVal(char recordChar, int recordSize);
	virtual void connect();
	virtual double initializeDB();
	virtual double readEntry(bool randomOption);
	virtual double insertEntry(int key);
	virtual double updateEntry(int key, bool randomOption);
	virtual double deleteEntry(int key, bool randomOption);
	virtual double simultaneousTasks(bool randomOption);
	virtual double performTransactions(int key);
	
	std::vector<int> getRandomKeys(int len, int min, int max);
	void setThreads(int n);
	void setEntries(int n);
	void setRecordSize(int n);
	int getThreads();
	int getEntries();

    protected:
	double calculateTime(std::chrono::time_point<std::chrono::high_resolution_clock> start, std::chrono::time_point<std::chrono::high_resolution_clock> end); 
	int threads;
	int entries;
	int recordSize;
	
	
};

#endif
