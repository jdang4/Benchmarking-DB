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
 */
char* DBClient::getEntryVal(char recordChar, int recordSize)
{
    char* val = new char[recordSize];
    strncpy(val, "", 1);

    for (int i = 0; i < recordSize; i++)
    {
        const char* str_letter = &recordChar;
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
 */
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
 */
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

/**
 * sets the threads
 * 
 * @param n - the number to set for threads
 */
void DBClient::setThreads(int n)
{
    threads = n;
}


/**
 * sets the entries
 * 
 * @param n - the number to set for entries
 */
void DBClient::setEntries(int n)
{
    entries = n;
}


/**
 * sets the record size
 * 
 * @param n - the number to set for recordSize
 */
void DBClient::setRecordSize(int n)
{
    recordSize = n;
}


/**
 * returns the threads
 * 
 * @return the value set for threads
 */
int DBClient::getThreads()
{
    return threads;
}


/**
 * returns the entries
 * 
 * @return the value set for entries
 */
int DBClient::getEntries()
{
    return entries;
}


/**
 * makes a connection to the DB
 * 
 */
void DBClient::connect()
{
    cout << "Method is ran from the DBClient...." << endl;
    exit(1);
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
double DBClient::readEntry(bool randomOption) 
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
double DBClient::insertEntry(int key)
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
double DBClient::updateEntry(int key, bool randomOption) 
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
double DBClient::deleteEntry(int key, bool randomOption)
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
double DBClient::simultaneousTasks(bool randomOption)
{
    cout << "Method is ran from the DBClient...." << endl;
    return -1;
}


/**
 * Simulates having multiple of transactions happening at the same time.
 * A transactin consists of performing an insert and then a delete.
 *
 * @param n - the number of simultaneous threads
 * @double p - the success percentage
 *
 * @return a double value of the time it took for all the 
 * simultaneous transactions going on
 */
double DBClient::performTransactions(int key)
{
    cout << "Method is ran from the DBClient...." << endl;
    return -1;
}
