#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include "headers/PostgresClient.h"

using namespace std;
using namespace pqxx;

PostgresClient::PostgresClient(string host_name) : DBClient() 
{
    dataVal = DBClient::getEntryVal('a');
    newVal = DBClient::getEntryVal('j');
    connection_description = "dbname = SDB user = postgres password = Juni#20 \
		hostaddr = " + host_name + " port = 5432";
}

PostgresClient::~PostgresClient() {}



/*
 * see description at DBClient::connect
 */
void PostgresClient::connect() 
{
    try {
		connection* postgres = new connection(connection_description);

		if (postgres->is_open())
		{
			cout << "CONNECTED TO POSTGRESQL!\n" << endl;
			postgres->disconnect();
		}

		else 
		{
			cout << "CONNECTION TO POSTGRESQL FAILED!\n" << endl;
			exit(-1);
		}

	} catch (const exception &e) {
		cerr << e.what() << endl;
		exit(-1);
	}
}


/*
 * see description at DBClient::disconnect
 */
void PostgresClient::disconnect()
{
    //postgres->disconnect();
    cout << "POSTGRESQL DISCONNECTED\n" << endl;
}


template<typename Lambda>
double PostgresClient::run_threads(Lambda f, int begin, bool random, int n)
{
    vector<thread> thread_pool;

    int threads = DBClient::getThreads();

    int runs = DBClient::getRuns();

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


/*
 * see description at DBClient::initializeDB()
 */
double PostgresClient::initializeDB()
{
    double time;

    try {
		connection* postgres = new connection(connection_description);
		
		string stmt = "DROP TABLE IF EXISTS session;";
		
		work query1(*postgres);
		
		query1.exec(stmt);

		query1.commit();

		work query2(*postgres);
		
		stmt = "CREATE TABLE session(" \
			"ID	INT	PRIMARY KEY	NOT NULL," \
			"DATA	TEXT	NOT NULL );";

		query2.exec(stmt);

		query2.commit();

		postgres->disconnect();
	
	} catch (const exception &e) {
		cerr << e.what() << endl;
		time = -1;
	}

	cout << "Creating the SDB..." << endl;

	auto createDB = [&](int start, int end, bool random) {
		try {
			mutex mtx;

			mtx.lock();
			connection* postgres = new connection(connection_description);

			for (int64_t i = start; i < end; i++)
			{
				string stmt = "INSERT INTO session (ID, DATA) VALUES (" + to_string(i) + ", '" + dataVal + "' );";
				
				work insertWork(*postgres);
				insertWork.exec(stmt);
				insertWork.commit();
			}

			postgres->disconnect();

			mtx.unlock();
		
		} catch (const exception &e) {
			cerr << e.what() << endl;
			exit(-1);
		}
	};

	time = run_threads(createDB, 1, false, 1000000);

    return time;
}

/*
 * see description at DBClient::readEntry
 */
double PostgresClient::readEntry(bool randomOption)
{
	auto read = [&](int start, int end, bool random) {
		try {
			mutex mtx;

			mtx.lock();

			connection* postgres = new connection(connection_description);

			for (int i = start; i < end; i++)
			{
				srand(time(0));
				int randomNum = (rand() % (end + 1 - start)) + start;  
				int randomKey = (random) ? randomNum : i;

				string key = to_string(randomKey);

				string stmt = "SELECT * FROM session WHERE ID = " + key + ";";

				work query(*postgres);

				query.exec(stmt);

				query.commit();
			}

			postgres->disconnect();

			mtx.unlock();

		} catch (const exception &e) {
			cerr << e.what() << endl;
			exit(-1);
		}
	};
    
	return run_threads(read, 1, randomOption);
}

/*
 * see description at DBClient::insertEntry
 */
double PostgresClient::insertEntry()
{
	auto insert = [&](int start, int end, bool random) {
		try {
			mutex mtx;

			mtx.lock();

			connection* postgres = new connection(connection_description);

			for (int i = start; i < end; i++)
			{
				string key = to_string(i);

				string stmt = "INSERT INTO session (ID, DATA) VALUES (" \
				+ key + ", '" + dataVal + "' );";

				work query(*postgres);

				query.exec(stmt);

				query.commit();
			}

			postgres->disconnect();

			mtx.unlock();

		} catch (const exception &e) {
			cerr << e.what() << endl;
			exit(-1);
		}
	};
    
	return run_threads(insert, 2000000, false);
}


/*
 * see description at DBClient::updateEntry
 */
double PostgresClient::updateEntry(bool randomOption)
{
	auto update = [&](int start, int end, bool random) {
		try {
			mutex mtx;

			mtx.lock();

			connection* postgres = new connection(connection_description);

			for (int i = start; i < end; i++)
			{
				srand(time(0));
				int randomNum = (rand() % (end + 1 - start)) + start;  
				int randomKey = (random) ? randomNum : i;

				string key = to_string(randomKey);

				string stmt = "UPDATE session set DATA = '";

				stmt.append(newVal);

				stmt += "' WHERE ID = " + key + ";";

				work query(*postgres);

				query.exec(stmt);

				query.commit();
			}

			postgres->disconnect();

			mtx.unlock();

		} catch (const exception &e) {
			cerr << e.what() << endl;
			exit(-1);
		}
	};
    
	return run_threads(update, 2000000, randomOption);
}


/*
 * see description at DBClient::deleteEntry
 */
double PostgresClient::deleteEntry(bool randomOption)
{
    auto deletion = [&](int start, int end, bool random) {
		try {
			connection* postgres = new connection(connection_description);

			for (int i = start; i < end; i++)
			{
				srand(time(0));
				int randomNum = (rand() % (end + 1 - start)) + start;  
				int randomKey = (random) ? randomNum : i;

				string key = to_string(randomKey);

				string stmt = "DELETE FROM session WHERE ID = " + key + ";";

				work query(*postgres);

				query.exec(stmt);

				query.commit();
			}

			postgres->disconnect();
		
		} catch (const exception &e) {
			cerr << e.what() << endl;
			exit(-1);
		}
	};
    
	return run_threads(deletion, 2000000, randomOption);
}


/*
 * see description at DBClient::simultaneousTasks
 */
double PostgresClient::simultaneousTasks(bool randomOption)
{
	auto read_and_write = [&](int start, int end, bool random) {
		try {
			connection* postgres = new connection(connection_description);

			auto read = [&](int aKey) {
				string key = to_string(aKey);

				string stmt = "SELECT * FROM session WHERE ID = " + key + ";";

				work query(*postgres);

				query.exec(stmt);

				query.commit();
			};

			auto write = [&](int aKey) {
				string key = to_string(aKey);

				string stmt = "UPDATE session set DATA = '";

				stmt.append(newVal);

				stmt += "' WHERE ID = " + key + ";";

				work query(*postgres);

				query.exec(stmt);

				query.commit();
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

			postgres->disconnect();

		} catch (const exception &e) {
			cerr << e.what() << endl;
			exit(-1);
		}
	};

	return run_threads(read_and_write, 1, randomOption);
}


/*
 * see description at DBClient::performTransactions
 */
double PostgresClient::performTransactions(bool randomOption)
{
	auto transaction = [&](int start, int end, bool random) {
		try {
			connection* postgres = new connection(connection_description);

			auto success = [&](int64_t aKey) {
				string key = to_string(aKey);
				work query(*postgres);
				
				string stmt = "INSERT INTO session (ID, DATA) VALUES (" \
				+ key + ", '" + dataVal + "' );";

				query.exec(stmt);

				for (int i = 0; i < 8; i++)
				{
					srand(time(0));
					int randomKey = (rand() % (1000000 + 1 - 1)) + 1;
					string tempKey = to_string(randomKey);

					if (i < 4)
					{
						stmt = "SELECT * FROM session WHERE ID = " + tempKey + ";";
						query.exec(stmt);
					}

					else 
					{
						stmt = "UPDATE session set DATA = '";
						
						stmt.append(newVal);
						
						stmt += "' WHERE ID = " + tempKey + ";";
					}
				}

				stmt = "SELECT * FROM session WHERE ID = " + key + ";";
				query.exec(stmt);

				stmt = "DELETE FROM session WHERE ID = " + key + ";";
				query.exec(stmt);
			};

			auto fail = [&](int64_t aKey) {
				string key = to_string(aKey);
				work query(*postgres);

				string stmt = "INSERT INTO session (ID, DATA) VALUES (" \
				+ key + ", '" + dataVal + "' );";

				query.exec(stmt);

				stmt = "DELETE FROM session WHERE ID = " + key + ";";
				query.exec(stmt);
			};

			int range = end - start;

			int numOfSuccess = range * 0.7;

			for (int64_t i = start; i < end; i++)
			{
				if (i < numOfSuccess)
				{
					success(i);
				}

				else
				{
					fail(i);
				}
				
			}

			postgres->disconnect();
		
		} catch (const exception &e) {
			cerr << e.what() << endl;
			exit(-1);
		}
	};

	return run_threads(transaction, 2000000, randomOption);
}
