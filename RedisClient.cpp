#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <exception>
#include "headers/RedisClient.h"

using namespace sw::redis;
using namespace std;


RedisClient::RedisClient() : DBClient()
{
    dataVal = DBClient::getEntryVal('a');
    newVal = DBClient::getEntryVal('j');

}


RedisClient::~RedisClient() {}


/**
 * see description at DBClient::connect()
 */
void RedisClient::connect()
{
    try {
	redis = new Redis("tcp://redis-master:6379");
	 
	// verifying the connection by sending a command
	redis->set("foo", "bar");


	cout << "CONNECTED TO REDIS!\n" << endl;
    
    } catch (const Error &e) {
	cout << "UNABLE TO CONNECT TO REDIS\n" << endl;
	exit(1);
    }
}


template<typename Lambda>
double RedisClient::run_threads(Lambda f, int begin, bool random, int n)
{
    vector<thread> thread_pool;
    int threads = DBClient::getThreads();

    int runs = DBClient::getRuns();

    cout << "Threads: " << threads << endl;
    cout << "Runs: " << runs << endl;

    int numOfRuns = (n == 0) ? runs : n;

    int perThread = numOfRuns / threads;

    int remainingThreads = numOfRuns % threads;

    int beginRange, endRange;

    int runningCount = begin;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < threads; i++)
    {
        beginRange = runningCount;
        endRange = beginRange + perThread;

        if (remainingThreads > 0)
        {
            remainingThreads--;
            endRange++;
        }

        thread_pool.push_back(thread(f, beginRange, endRange, random));

        runningCount = endRange;
    }

    for (auto &thread : thread_pool)
    {
        thread.join();
    }

    auto end = chrono::high_resolution_clock::now();

    return DBClient::calculateTime(start, end);
}


/**
 * see description at DBClient::initializeDB()
 */
double RedisClient::initializeDB() 
{
    double time;
    
    try {
		redis->command<void>("flushall");

		cout << "Creating the SDB..." << endl;
	
    } catch (const Error &e) {
		cout << "ERROR DURING INITIALIZATION" << endl;
		time = -1.0;
    }


    auto createDB = [&](int start, int end, bool random) {
		try {
	    	for (int64_t i = start; i < end; i++) 
	    	{
				auto key = to_string(i);
				redis->set(key, dataVal);
	    	}
	
		} catch (const Error &e) {
	    	cout << "ERROR CREATING THE DATABASE" << endl;
	    	exit(-1);
		}
    };

    time = run_threads(createDB, 1, false, 1000000);

    cout << "SDB Creation Completed!\n" << endl;

    return time;
}


/**
 * see a description at DBClient::readEntry
 */
double RedisClient::readEntry(bool randomOption)
{
    auto read = [&](int start, int end, bool random) {
        try {
            for (int i = start; i < end; i++)
            {
				srand(time(0));
				int randomNum = (rand() % (end + 1 - start)) + start;  
				int key = (random) ? randomNum : i;

                redis->get(to_string(key));
            }
        
        } catch (const Error &e) {
            cerr << "ERROR DURING READ" << endl;
            exit(-1);
        }
    };
    
    return run_threads(read, 1, randomOption);
}


/**
 * see description at DBClient::insertEntry()
 */
double RedisClient::insertEntry()
{
    auto insert = [&](int start, int end, bool random) {
        try {
            for (int i = start; i < end; i++)
            {
                redis->set(to_string(i), dataVal);
            }
        
        } catch (const Error &e) {
            cerr << "ERROR DURING INSERT" << endl;
            exit(-1);
        }
    };

    return run_threads(insert, 2000000, false);
}

/**
 * see description at DBClient::updateEntry()
 */
double RedisClient::updateEntry(bool randomOption) 
{
    auto update = [&](int start, int end, bool random) {
		try {
	    	for (int i = start; i < end; i++)
	    	{
				srand(time(0));
				int randomNum = (rand() % (end + 1 - start)) + start;  
				int key = (random) ? randomNum : i;
				redis->set(to_string(key), newVal);
	    	}
	
		} catch (const Error &e) {
	    	cerr << "ERROR DURING UPDATE" << endl;
	    	exit(-1);
		}
	};
	
	return run_threads(update, 2000000, randomOption);
}


/**
 * see description at DBClient::deleteEntry()
 */
double RedisClient::deleteEntry(bool randomOption)
{
    auto deletion = [&](int start, int end, bool random) {
	try {
	    for (int i = start; i < end; i++)
	    {
		srand(time(0));
		int randomNum = (rand() % (end + 1 - start)) + start;  
		int key = (random) ? randomNum : i;

		redis->del(to_string(key));
	    }
	
	} catch (const Error &e) {
	    cerr << "ERROR DURING DELETION" << endl;
	    exit(-1);
	}
    };

    return run_threads(deletion, 2000000, randomOption);
}


/*
 * see description at DBClient::simultaneousTasks()
 */
double RedisClient::simultaneousTasks(bool randomOption)
{
    auto read_and_write = [&](int start, int end, bool random) {
	
	auto read = [&](int key) {
	    try {
			redis->get(to_string(key));
	    
	    } catch (const Error &e) {
		cout << "ERROR READING" << endl;
		exit(-1);
	    }
	};

	auto write = [&](int key) {
	    try {
			redis->set(to_string(key), newVal);
	    
	    } catch (const Error &e) {
			cout << "ERROR UPDATING" << endl;
			exit(-1);
	    }
	};

	int halfMark = (end - start) / 2;

	for (int i = start; i < end; i++) 
	{
	    srand(time(0));
	    int randomNum = (rand() % (end + 1 - start)) + start;  
	    int key = (random) ? randomNum : i;

	    if (i < halfMark)
	    {
			read(key);
	    }

	    else
	    {
			write(key);
	    }
	}

    };

    return run_threads(read_and_write, 1, randomOption);
}


/*
 * see description at DBClient::performTransactions()
 */
double RedisClient::performTransactions(bool randomOption)
{
    auto transaction = [&](int start, int end, bool random) {

		auto success = [&](int64_t aKey) {
	    	try {
				redis->set(to_string(aKey), dataVal);

				for (int i = 0; i < 8; i++) 
				{
		    		srand(time(0));
		    		int randomKey = (rand() % (1000000 + 1 - 1)) + 1;

		    		if (i < 4) 
			    	{
						redis->get(to_string(randomKey));
			    	}	

		    		else
		    		{
						redis->set(to_string(randomKey), newVal);
			    	}
				}

				redis->get(to_string(aKey));

				redis->del(to_string(aKey));
	    
		    } catch (const Error &e) {
				cout << "ERROR DURING TRANSACTION SUCCESS" << endl;
				exit(-1);
	    	}
		};

		auto fail = [&](int64_t aKey) {
	    	try {
				redis->set(to_string(aKey), dataVal);

				redis->del(to_string(aKey));
	    
	    	} catch (const Error &e) {
				cout << "ERROR DURING TRANSACTION FAIL" << endl;
				exit(-1);
	    	}
		};

		int range = end - start;

		int numOfSuccess = range * 0.7;

		for (int64_t i = start; i < end; i++) 
		{
	    	int index = i;
	    	if (index < numOfSuccess)
	    	{
				success(i);
	    	}

	    	else
	    	{
				fail(i);
	    	}
		}
    };

    return run_threads(transaction, 2000000, randomOption);
}
