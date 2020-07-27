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
	virtual double readEntry(bool randomOption);
	virtual double insertEntry(int key);
	virtual double updateEntry(int key, bool randomOption);
	virtual double deleteEntry(int key, bool randomOption);
	virtual double simultaneousTasks(bool randomOption);
	virtual double performTransactions(int key);
	
	std::vector<int> getRandomKeys(int len, int min, int max);
	void setThreads(int n);
	void setRuns(int n);
	int getThreads();
	int getRuns();


    protected:
	double calculateTime(std::chrono::time_point<std::chrono::high_resolution_clock> start, std::chrono::time_point<std::chrono::high_resolution_clock> end); 
	
	private:
	int threads;
	int runs;
};

#endif
