#include <iostream>
#include <thread>
#include <vector>
#include <set>
#include <chrono>
#include "headers/RedisClient.h"

using namespace sw::redis;
using namespace std;

/**
 * constructor 
 */
RedisClient::RedisClient(bool snap, bool append) : DBClient()
{
    rdb = snap;
    aof = append;

    entryVal = DBClient::getEntryVal('a');
    newVal = DBClient::getEntryVal('j');

    createConnection(); 
}


/**
 * destructor
 */
RedisClient::~RedisClient() {}

/*
double RedisClient::calculateTime(chrono::time_point<std::chrono::high_resolution_clock> start, chrono::time_point<std::chrono::high_resolution_clock> end)
{
    auto elapsed = chrono::duration_cast<chrono::microseconds>(end - start);

    double time = elapsed.count() * 1e-6;

    return time;
}
*/

/**
 * creates the connection to the running redis server (master) in a running container
 */
void RedisClient::createConnection()
{
    try {
	redis = new Redis("tcp://redis:6379");
	
	if (!aof)
	{
	    redis->command<void>("config", "set", "appendonly", "no");
	}

	if (!rdb)
	{
	    redis->command<void>("config", "set", "save", "");
	}

	if (!rdb || !aof)
	{
	    cout << "re-write" << endl;
	    redis->command<void>("config", "rewrite");
	}
	 
	// verifying the connection by sending a command
	redis->set("foo", "bar");


	cout << "CONNECTED TO REDIS!\n" << endl;
    
    } catch (const Error &e) {
	cout << "UNABLE TO CONNECT TO REDIS\n" << endl;
	exit(1);
    }
}


/**
 * creates a replica of the actual sdb by inserting 1 million entries with
 * each having a length of 6K bytes
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
 * does a simple redis get (essentially doing a READ) on a given key
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
 * inserting an entry with the given key and automatically assigning
 * a 6K byte val to the entry
 *
 * @param key - the string value of the key to be inserted
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
 * doing an update on an existing entry in the Redis DB. The method
 * does verify if the key exists in order to differ from the insertEntry
 * method
 *
 * @param key - the string value of the key to be updated
 * @param newVal - the string value of the updated value
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
 * deletes the entry with a key that matches the given key value
 *
 * @param key - the string value of the key to be deleted
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
set<int> RedisClient::getRandomKeys(int n, int lower, int upper)
{
    srand(time(0));
    set<int> keySet;
    int count = 0;

    while (count != n)
    {
	// get a random num from 1 to 1 million
	int randomNum = (rand() % (upper + 1 - lower)) + lower;

	auto it = keySet.find(randomNum);

	// if key is not already in the set
	if (it == keySet.end() && randomNum <= 1000000)
	{
	    keySet.insert(randomNum);
	    count++;
	}
    }

    return keySet;
}
*/
double RedisClient::simultaneousTasks(int n)
{
    set<int> keySet = DBClient::getRandomKeys(n, 1, 100000);

    int randomKeys[n];

    copy(keySet.begin(), keySet.end(), randomKeys);

    vector<thread> thread_pool;

    auto read = [&](string aKey) {
	readEntry(aKey);
    };

    auto write = [&](string aKey) {
	updateEntry(aKey);
    };

    if (n <= 0)
    {
	return -1.0;
    }

    else if (n == 1)
    {
	return readEntry(to_string(randomKeys[0]));
    }

    auto start = chrono::high_resolution_clock::now();
    
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



double RedisClient::performTransactions(int n, double successPercentage)
{
    if (successPercentage < 0 || successPercentage > 100 || n <= 0)
    {
	return -1.0;
    }

    int numOfSuccess = n * successPercentage;

    auto success = [&] (string aKey)
    {
	insertEntry(aKey);
	simultaneousTasks(8);
	readEntry(aKey);
	deleteEntry(aKey);
    };

    auto fail = [&] (string aKey)
    {
	insertEntry(aKey);
	deleteEntry(aKey);
    };

    vector<thread> thread_pool;

    int64_t key = 2000000;


    auto start = chrono::high_resolution_clock::now();

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



