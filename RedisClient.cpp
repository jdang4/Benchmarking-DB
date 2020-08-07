#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <exception>
#include "headers/RedisClient.h"

using namespace sw::redis;
using namespace std;


RedisClient::RedisClient(string host_name) : DBClient()
{
    dataVal = DBClient::getEntryVal('a');
    newVal = DBClient::getEntryVal('j');
    
    options.host = host_name;
    options.port = 6379;

    pool_options.size = 50;
}


RedisClient::~RedisClient() {}


/**
 * see description at DBClient::connect()
 */
void RedisClient::connect()
{
	try
	{
		Redis *redis = new Redis(options, pool_options);

		// verifying the connection by sending a command
		redis->set("foo", "bar");

		redis->del("foo");

		cout << "CONNECTED TO REDIS!\n" << endl;

		redis->command<void>("quit");
	}
	
	catch (const Error &e)
	{
		cout << "UNABLE TO CONNECT TO REDIS\n" << endl;
		exit(1);
	}
}

/**
 * This method is used to create the running threads. It first calculates how many
 * entries are allocated to each thread. Once it completes that it would then add 
 * a thread that performs the provided lambda function within its given range into the 
 * thread pool. After creating and adding the threads to the pool it would wait until
 * all are finsihed and it is timing how long it takes to do so
 * 
 * @tparam Lambda 
 * @param f - the lambda function to have each thread run
 * @param begin - the starting entry key
 * @param random - the ranomize key option
 * @param n - used to define how many entries in total will be handled. It is predefined to 0
 * 	- if n == 0 ==> handle the number of entries that the user have defined
 * - if n != 0 ==> handle the number of entries that is defined by this parameter
 * 
 * @return the time it took to complete all the threads that were used  
 */
template<typename Lambda>
double RedisClient::run_threads(Lambda f, int begin, bool random, int n)
{
    vector<thread> thread_pool;
    int threads = DBClient::getThreads();

    int runs = DBClient::getRuns();

    int numOfRuns = (n == 0) ? runs : n;

    threads = (threads > numOfRuns) ? numOfRuns : threads; 

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
		Redis* redis = new Redis(options, pool_options);
 		    
		redis->command<void>("flushall");

		redis->command<void>("quit");
		

		cout << "Creating the SDB..." << endl;
	
    } catch (const Error &e) {
		cout << "ERROR DURING INITIALIZATION" << endl;
		cerr << e.what() << endl;
		time = -1.0;
    }


    auto createDB = [&](int start, int end, bool random) {
		try {
			Redis* redis = new Redis(options, pool_options);

	    	for (int64_t i = start; i < end; i++) 
	    	{
				auto key = to_string(i);
				redis->set(key, dataVal);
	    	}

			redis->command<void>("quit");

	
		} catch (const Error &e) {
	    	cout << "ERROR CREATING THE DATABASE" << endl;
			cerr << e.what() << endl;
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
		try
		{

			Redis *redis = new Redis(options, pool_options);

			for (int i = start; i < end; i++)
			{
				srand(time(0));

				int threads = DBClient::getThreads();
				int runs = DBClient::getRuns();

				int maxNum = runs / threads;

				if (threads > runs)
				{
					maxNum = threads;
				}

				int randomNum = (rand() % maxNum) + 1;

				int key = (random) ? randomNum : i;

				redis->get(to_string(key));
			}

			redis->command<void>("quit");
		}
		catch (const Error &e)
		{
			cerr << "ERROR DURING READ" << endl;
			cerr << e.what() << endl;
			exit(-1);
		}
	};

	return run_threads(read, 1, randomOption);
}

/**
 * see description at DBClient::insertEntry()
 */
double RedisClient::insertEntry(int key)
{
    auto insert = [&](int start, int end, bool random) {
        try {
		
	    	Redis* redis = new Redis(options, pool_options);

            for (int64_t i = start; i < end; i++)
            {
                redis->set(to_string(i), dataVal);
            }
			
			redis->command<void>("quit");
	    
        
        } catch (const Error &e) {
            cerr << "ERROR DURING INSERT" << endl;
			cerr << e.what() << endl;
            exit(-1);
        }
    };

    return run_threads(insert, key, false);
}

/**
 * see description at DBClient::updateEntry()
 */
double RedisClient::updateEntry(int key, bool randomOption)
{
	auto update = [&](int start, int end, bool random) {
		try
		{
			Redis *redis = new Redis(options, pool_options);

			for (int i = start; i < end; i++)
			{
				srand(time(0));
				int threads = DBClient::getThreads();
				int runs = DBClient::getRuns();

				int maxNum = runs / threads;

				if (threads > runs)
				{
					maxNum = threads;
				}

				int randomNum = (rand() % maxNum) + 1;

				int key = (random) ? randomNum : i;

				redis->set(to_string(key), newVal);
			}

			redis->command<void>("quit");
		}
		catch (const Error &e)
		{
			cerr << "ERROR DURING UPDATE" << endl;
			cerr << e.what() << endl;
			exit(-1);
		}
	};

	return run_threads(update, key, randomOption);
}

/**
 * see description at DBClient::deleteEntry()
 */
double RedisClient::deleteEntry(int key, bool randomOption)
{
	auto deletion = [&](int start, int end, bool random) {
		try
		{
			Redis *redis = new Redis(options, pool_options);

			for (int i = start; i < end; i++)
			{
				srand(time(0));
				int threads = DBClient::getThreads();
				int runs = DBClient::getRuns();

				int maxNum = runs / threads;

				if (threads > runs)
				{
					maxNum = threads;
				}

				int randomNum = (rand() % maxNum) + 1;

				int key = (random) ? randomNum : i;

				redis->del(to_string(key));
			}

			redis->command<void>("quit");
		}
		catch (const Error &e)
		{
			cerr << "ERROR DURING DELETION" << endl;
			cerr << e.what() << endl;
			exit(-1);
		}
	};

	return run_threads(deletion, key, randomOption);
}

/*
 * see description at DBClient::simultaneousTasks()
 */
double RedisClient::simultaneousTasks(bool randomOption)
{
	auto read_and_write = [&](int start, int end, bool random) {
		Redis *redis = new Redis(options, pool_options);

		auto read = [&](int key) {
			try
			{
				redis->get(to_string(key));
			}
			catch (const Error &e)
			{
				cout << "ERROR READING" << endl;
				cerr << e.what() << endl;
				exit(-1);
			}
		};

		auto write = [&](int key) {
			try
			{
				redis->set(to_string(key), newVal);
			}
			catch (const Error &e)
			{
				cout << "ERROR UPDATING" << endl;
				cerr << e.what() << endl;
				exit(-1);
			}
		};

		int halfMark = (end - start) / 2;

		for (int i = start; i < end; i++)
		{
			srand(time(0));
			int threads = DBClient::getThreads();
			int runs = DBClient::getRuns();

			int maxNum = runs / threads;

			if (threads > runs)
			{
				maxNum = threads;
			}

			int randomNum = (rand() % maxNum) + 1;

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

		redis->command<void>("quit");
	};

	return run_threads(read_and_write, 1, randomOption);
}


/*
 * see description at DBClient::performTransactions()
 */
double RedisClient::performTransactions(int key)
{
    auto transaction = [&](int start, int end, bool random) {
		Redis* redis = new Redis(options, pool_options);

		auto write_and_delete = [&](int64_t key) {
			try {
				redis->set(to_string(key), dataVal);
				
				redis->del(to_string(key));
	    
	    	} catch (const Error &e) {
				cout << "ERROR UPDATING AND DELETING" << endl;
				cerr << e.what() << endl;
				exit(-1);
	    	}
		};

		for (int64_t i = start; i < end; i++) 
		{
			write_and_delete(i);
		}

		redis->command<void>("quit");
    };

    return run_threads(transaction, key, false);
}
