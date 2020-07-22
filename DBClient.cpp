#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include "headers/DBClient.h"

using namespace std;


/**
 * This method creates a value with a size of 6K bytes
 *
 * @return a pointer to a char array with a size of 6K bytes
 **/
char* DBClient::getEntryVal(char startingChar)
{
    char* val = new char[ENTRY_SIZE];
    strncpy(val, "", 1);

    int letter = int(startingChar);

    for (int i = 0; i < ENTRY_SIZE; i++)
    {
	if (i != 0 && i % 1200 == 0)
	{
	    letter++;
	}

	char c = char(letter);
	const char* str_letter = &c;
	strncat(val, str_letter, 1);
    }

    return val;
}

/**
 * This method calculates the elapsed time based on the given start and end time values 
 *
 * @param start - the starting timestamp
 * @param end - the ending timestamp
 *
 * @return a double value of the elapsed time
 **/
double DBClient::calculateTime(chrono::time_point<std::chrono::high_resolution_clock> start, chrono::time_point<std::chrono::high_resolution_clock> end)
{
    auto elapsed = chrono::duration_cast<chrono::microseconds>(end - start);

    double time = elapsed.count() * 1e-6;

    return time;
}


/**
 * This method generates n random numbers within the lower to upper range
 * and none of those random numbers are repeated
 *
 * @param n - the number of random numbers to generate
 * @param min - the minimum number that the random number can be
 * @param max - the maximum number that the random number can be
 *
 * @return - returns a set of n random numbers
 **/
vector<int> DBClient::getRandomKeys(int n, int lower, int upper)
{
    srand(time(0));
    vector<int> keySet;
    //int count = 0;

    for (int i = 0; i < n; i++)
    {
	int randomNum = (rand() % (upper + 1 - lower)) + lower;

	keySet.push_back(randomNum);
    }

    return keySet;
}

void DBClient::setThreads(int n)
{
    threads = n;
}

void DBClient::setRuns(int n)
{
    runs = n;
}


void DBClient::connect()
{
    cout << "Method is ran from the DBClient...." << endl;
    exit(1);
}


void DBClient::disconnect() 
{
    cout << "DB disconnected\n" << endl;
}

/**
 * Initializes the DB by inserting 1 million entries with each having a data 
 * of 6K bytes and a key of 64 bits
 *
 * @return the double value of the time it took to initialize the DB
 **/
double DBClient::initializeDB()
{
    cout << "Method is ran from the DBClient...." << endl;
    return -1;
}

/**
 * Does a simple read on an entry in the DB
 *
 * @param key - the key of the entry to be read
 *
 * @return the double value of the time it took to complete a read
 **/
double DBClient::readEntry(string key, bool randomOption) 
{
    cout << "Method is ran from the DBClient...." << endl;
    return -1;
}


/**
 * Does an insertion of an entry into the DB
 *
 * @param key - the key of the entry to be inserted into the DB
 *
 * @return the double value of the time it took to complete the insertion
 **/
double DBClient::insertEntry(string key)
{
    cout << "Method is ran from the DBClient...." << endl;
    return -1;
}


/**
 * Makes an update on an existing entry in the DB
 *
 * @param key - the key of the entry to update
 * @param newVal - the value to update the provided key
 *
 * @return a double value of the time it took to complete an update
 **/
double DBClient::updateEntry(string key, bool randomOption) 
{
    cout << "Method is ran from the DBClient...." << endl; 
    return -1;
}


/**
 * Deletes an entry within the DB
 *
 * @param key - the key of the entry to be deleted
 *
 * @return a double value of the time it took to complete a deletion
 **/
double DBClient::deleteEntry(string key, bool randomOption)
{
    cout << "Method is ran from the DBClient...." << endl;
    return -1;
}


/**
 * Simulates having multiple of tasks happening at the same time.
 * The tasks done are either reading or doing an update on an entry. 
 * The reading/modifying is split 50:50 and is based on the number 
 * of simultaneous tasks going on.
 *
 * @param n - the number of simultaneous tasks
 *
 * @return a double value of the time it took for all the 
 * simultaneous tasks to finish
 **/
double DBClient::simultaneousTasks(int n, bool randomOption)
{
    cout << "Method is ran from the DBClient...." << endl;
    return -1;
}


/**
 * Simulates having multiple of transactions happening at the same time.
 * The transactions done are either a success or a failure. A success transaction performs
 * 1 insert, 8 simultaneous read/modify, and 1 delete. A fail transaction performs 1 insert and
 * 1 delete.
 *
 * @param n - the number of simultaneous threads
 * @double p - the success percentage
 *
 * @return a double value of the time it took for all the 
 * simultaneous transactions going on
 */
double DBClient::performTransactions(int n, double p, bool randomOption)
{
    cout << "Method is ran from the DBClient...." << endl;
    return -1;
}
