#include <iostream>
#include <thread>
#include <vector>
#include <set>
#include <chrono>
#include "headers/RedisClient.h"

using namespace sw::redis;
using namespace std;


RedisClient::RedisClient() : DBClient()
{
    entryVal = DBClient::getEntryVal('a');
    newVal = DBClient::getEntryVal('j');
}


RedisClient::~RedisClient() {}


/**
 * see description at DBClient::connect()
 */
void RedisClient::connect()
{
    try {
	redis = new Redis("tcp://redis:6379");
	 
	// verifying the connection by sending a command
	redis->set("foo", "bar");


	cout << "CONNECTED TO REDIS!\n" << endl;
    
    } catch (const Error &e) {
	cout << "UNABLE TO CONNECT TO REDIS\n" << endl;
	exit(1);
    }
}


/**
 * see description at DBClient::initializeDB()
 */
double RedisClient::initializeDB() 
{
    double time;
    
    try {

	auto start = chrono::high_resolution_clock::now();

	cout << "Creating the SDB..." << endl;

	redis->command<void>("flushall");

	for (int64_t i = 1; i <= NUM_OF_ENTRIES; i++) 
	{
	    auto key = to_string(i);
	    redis->set(key, entryVal);
	}

	auto end = chrono::high_resolution_clock::now();

	cout << "SDB Creation Completed!\n" << endl;

	time = DBClient::calculateTime(start, end);
	
    } catch (const Error &e) {
	cout << "ERROR DURING INITIALIZATION" << endl;
	time = -1.0;
    }

    return time;
}

/**
 * see a description at DBClient::readEntry
 */
double RedisClient::readEntry(string key)
{
    try {

	auto start = chrono::high_resolution_clock::now();

	auto val = redis->get(key);
	
	auto end = chrono::high_resolution_clock::now();
	
	return DBClient::calculateTime(start, end);
    
    } catch (const Error &e) {
	cout << "ERROR DURING READ" << endl;
    }

    return -1.0;
}


/**
 * see description at DBClient::insertEntry()
 */
double RedisClient::insertEntry(string key)
{
    try {

	auto start = chrono::high_resolution_clock::now();

	redis->set(key, entryVal);

	auto end = chrono::high_resolution_clock::now();

	return DBClient::calculateTime(start, end);
 
    } catch (const Error &e) {
	cout << "ERROR DURING INSERT" << endl;
    }

    return -1.0;
}

/**
 * see description at DBClient::updateEntry()
 */
double RedisClient::updateEntry(string key) 
{
    try {
	auto start = chrono::high_resolution_clock::now();

	auto val = redis->get(key);

	if (val) 
	{
	    redis->set(key, newVal);
	    
	    auto end = chrono::high_resolution_clock::now();

	    return DBClient::calculateTime(start, end);
	}

	else
	{
	    cout << key << " DOES NOT EXIST" << endl;
	}
    
    } catch (const Error &e) {
	cout << "ERROR DURING UPDATE" << endl;
    }

    return -1.0;
}

/**
 * see description at DBClient::deleteEntry()
 */
double RedisClient::deleteEntry(string key)
{
    try {
	auto start = chrono::high_resolution_clock::now();

	redis->del(key);

	auto end  = chrono::high_resolution_clock::now();

	return DBClient::calculateTime(start, end);
    
    } catch (const Error &e) {
	cout << "ERROR DURING DELETION" << endl;
    }

    return -1.0;
}

/*
 * see description at DBClient::simultaneousReaders
 */
double RedisClient::simultaneousReaders(int n, string key)
{
    vector<thread> thread_pool;

    auto read = [&](string aKey) {
	readEntry(aKey);
    };

    if (n <= 0)
    {
	return -1.0;
    }

    else if (n == 1)
    {
	return readEntry(key);
    }

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++)
    {
	thread_pool.push_back(thread(read, key));
    }

    for (auto &thread : thread_pool)
    {
	thread.join();
    }

    auto end = chrono::high_resolution_clock::now();
 
    return DBClient::calculateTime(start, end);

}

/*
 * see description at DBClient::simultaneousTasks()
 */
double RedisClient::simultaneousTasks(int n)
{
    set<int> keySet = DBClient::getRandomKeys(n, 1, 100000);

    int randomKeys[n];

    copy(keySet.begin(), keySet.end(), randomKeys);

    vector<thread> thread_pool;

    // defines what a read transaction composes of
    auto read = [&](string aKey) {
	readEntry(aKey);
    };

    // defines what a write transaction composes of
    auto write = [&](string aKey) {
	updateEntry(aKey);
    };

    if (n <= 0)
    {
	return -1.0;
    }

    // no need to use threads
    else if (n == 1)
    {
	return readEntry(to_string(randomKeys[0]));
    }

    auto start = chrono::high_resolution_clock::now();

    // idea each thread does either a read or write transaction
    for (int i = 0; i < n; i += 2)
    {
	auto readKey = to_string(randomKeys[i]);
	thread_pool.push_back(thread(read, readKey));

	if (i++ < n)
	{
	    auto writeKey = to_string(randomKeys[i++]);
	    thread_pool.push_back(thread(write, writeKey));
	}
    }

    for (auto &thread : thread_pool)
    {
	thread.join();
    }

    auto end = chrono::high_resolution_clock::now();

    return DBClient::calculateTime(start, end);
}


/*
 * see description at DBClient::performTransactions()
 */
double RedisClient::performTransactions(int n, double successPercentage)
{
    if (successPercentage < 0 || successPercentage > 100 || n <= 0)
    {
	return -1.0;
    }

    int numOfSuccess = n * successPercentage;

    // defines what the success transaction composes of
    auto success = [&] (string aKey)
    {
	insertEntry(aKey);

	// having simultaneous readers/modifiers for each transaction
	simultaneousTasks(8);

	readEntry(aKey);
	deleteEntry(aKey);
    };

    // defines what the fail transaction composes of
    auto fail = [&] (string aKey)
    {
	insertEntry(aKey);
	deleteEntry(aKey);
    };

    vector<thread> thread_pool;

    int64_t key = 2000000;

    auto start = chrono::high_resolution_clock::now();

    // each thread will either perform a success or a fail transaction
    for (int i = 0; i < n; i++)
    {
	if (i < numOfSuccess)
	{
	    auto aKey = to_string(key);
	    thread_pool.push_back(thread(success, aKey));
	}

	else
	{
	    auto aKey = to_string(key);
	    thread_pool.push_back(thread(fail, aKey));
	}

	key += 1;
    }
    
    for (auto &thread : thread_pool)
    {
	thread.join();
    }
    
    auto end = chrono::high_resolution_clock::now();

    return DBClient::calculateTime(start, end);
}    

