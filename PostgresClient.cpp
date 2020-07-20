#include <iostream>
#include <chrono>
#include <vector>
#include <thread>

#include "headers/PostgresClient.h"

using namespace std;
using namespace pqxx;

PostgresClient::PostgresClient(string host_name) : DBClient() 
{
    dataVal = DBClient::getEntryVal('a');
    newDataVal = DBClient::getEntryVal('j');
    host = host_name;
}

PostgresClient::~PostgresClient() {}


/*
 * see description at DBClient::connect
 */
void PostgresClient::connect() 
{
    try {
	postgres = new connection("dbname = SDB user = postgres password = Juni#20 \
		hostaddr = " + host + " port = 5432");

	if (postgres->is_open())
	{
	    cout << "CONNECTED TO POSTGRESQL!\n" << endl; 
	}

	else
	{
	    cout << "CONNECTING TO POSTGRESQL FAILED\n" << endl;
	    exit(1);
	}
	 
    } catch (const exception &e) {
	cerr << e.what() << endl;
	exit(1);
    }
}


/*
 * see description at DBClient::disconnect
 */
void PostgresClient::disconnect()
{
    postgres->disconnect();
    cout << "POSTGRESQL DISCONNECTED\n" << endl;
}


/*
 * see description at DBClient::initializeDB()
 */
double PostgresClient::initializeDB()
{
    double time;

    try {

	auto start = chrono::high_resolution_clock::now();

	string stmt = "DROP TABLE IF EXISTS session;";

	work dropWork(*postgres);

	dropWork.exec(stmt);

	dropWork.commit();
	
	cout << "Creating the SDB..." << endl;

	stmt = "CREATE TABLE session(" \
		"ID	INT	PRIMARY KEY	NOT NULL," \
		"DATA	TEXT	NOT NULL );";

	work tableWork(*postgres);
	
	tableWork.exec(stmt);

	tableWork.commit();

	for(int64_t i = 1; i <= NUM_OF_ENTRIES; i++)
	{
	    stmt = "INSERT INTO session (ID, DATA) VALUES (" + to_string(i) + ", '" + dataVal + "' );";

	    work insertWork(*postgres);

	    insertWork.exec(stmt);

	    insertWork.commit();
	}

	auto end = chrono::high_resolution_clock::now();

	cout << "SDB Creation Completed!\n" << endl;

	time = DBClient::calculateTime(start, end);
    
    } catch (const exception &e) {
	cerr << e.what() << endl;
	time = -1.0;
    }

    return time;
}

/*
 * see description at DBClient::readEntry
 */
double PostgresClient::readEntry(string key)
{
    try {

	work query(*postgres);

	auto start = chrono::high_resolution_clock::now();

	string stmt = "SELECT * FROM session WHERE ID = " + key + ";";

	query.exec(stmt);

	query.commit();

	auto end = chrono::high_resolution_clock::now();

	return DBClient::calculateTime(start, end);

    } catch (const exception &e) {
	cerr << e.what() << endl;
    }

    return -1;
}

/*
 * see description at DBClient::insertEntry
 */
double PostgresClient::insertEntry(string key)
{
    try {
	work query(*postgres);

	auto start = chrono::high_resolution_clock::now();

	string stmt = "INSERT INTO session (ID, DATA) VALUES (" \
		       + key + ", '" + dataVal + "' );";

	query.exec(stmt);

	query.commit();

	auto end = chrono::high_resolution_clock::now();

	return DBClient::calculateTime(start, end);
    
    } catch (const exception &e) {
	cerr << e.what() << endl;
    }

    return -1;
}


/*
 * see description at DBClient::updateEntry
 */
double PostgresClient::updateEntry(string key)
{
    try {
	work query(*postgres);

	auto start = chrono::high_resolution_clock::now();

	string stmt = "UPDATE session set DATA = '";
	
	stmt.append(newDataVal);
	
	stmt += "' WHERE ID = " + key + ";";

	query.exec(stmt);

	query.commit();

	auto end = chrono::high_resolution_clock::now();

	return DBClient::calculateTime(start, end);
    
    } catch (const exception &e) {
	cerr << e.what() << endl;
    }

    return -1;
}


/*
 * see description at DBClient::deleteEntry
 */
double PostgresClient::deleteEntry(string key)
{
    try {
	work query(*postgres);

	auto start = chrono::high_resolution_clock::now();

	string stmt = "DELETE FROM session WHERE ID = " + key + ";";

	query.exec(stmt);

	query.commit();

	auto end = chrono::high_resolution_clock::now();

	return DBClient::calculateTime(start, end);
    
    } catch (const exception &e) {
	cerr << e.what() << endl;
    }

    return -1;
}


/*
 * see description at DBClient::simultaneousReaders
 */
double PostgresClient::simultaneousReaders(int n, string key) 
{
    vector<thread> thread_pool;

    auto read = [&](string aKey) {
	try {

	    // idea to have each thread have its own connection to the DB (this client only singe-threaded)
	    connection* tmp = new connection("dbname = SDB user = postgres password = Juni#20 \
		hostaddr = " + host + " port = 5432");

	    string stmt = "SELECT * FROM session WHERE ID = " + aKey + ";";

	    work query(*tmp);

	    query.exec(stmt);

	    query.commit();

	    tmp->disconnect();

	} catch (const exception &e) {
	    cerr << e.what() << endl;
	    exit(1);
	}
    };

    if (n <= 0)
    {
	return -1;
    }

    // no need to use threads
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
 * see description at DBClient::simultaneousTasks
 */
double PostgresClient::simultaneousTasks(int n)
{
    vector<int> keySet = DBClient::getRandomKeys(n, 1, 1000000);

    int randomKeys[n];

    copy(keySet.begin(), keySet.end(), randomKeys);

    if (n <= 0)
    {
	return -1;
    }

    // no need to use threads
    else if (n == 1)
    {
	return readEntry(to_string(randomKeys[0]));
    }

    vector<thread> thread_pool;

    // defines what a read transaction composes of
    auto read = [&](string aKey) {
	try {
	    connection* tmp = new connection("dbname = SDB user = postgres password = Juni#20 \
		hostaddr = " + host + " port = 5432");

	    // performing a read
	    string stmt = "SELECT * FROM session WHERE ID = " + aKey + ";";

	    work query(*tmp);

	    query.exec(stmt);

	    query.commit();

	    tmp->disconnect();

	} catch (const exception &e) {
	    cerr << e.what() << endl;
	    exit(1);
	}

    };

    // defines what a write transaction composes of
    auto write = [&](string aKey) {
	try {
	    connection* tmp = new connection("dbname = SDB user = postgres password = Juni#20 \
		hostaddr = " + host + " port = 5432");

	    string stmt = "UPDATE session set DATA = '";
	
	    stmt.append(newDataVal);
	
	    stmt += "' WHERE ID = " + aKey + ";";

	    work query(*tmp);

	    query.exec(stmt);

	    query.commit();

	    tmp->disconnect();

	} catch (const exception &e) {
	    cerr << e.what() << endl;
	    exit(1);
	}

    };

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++)
    {
	auto key = to_string(randomKeys[i]);

	if (i < (n/2)) 
	{
	    thread_pool.push_back(thread(read, key));
	}

	else 
	{
	    thread_pool.push_back(thread(write, key));
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
 * see description at DBClient::performTransactions
 */
double PostgresClient::performTransactions(int n, double successPercentage)
{
    if (successPercentage < 0 || successPercentage > 100 || n <= 0)
    {
	return -1;
    }

    int numOfSuccess = n * successPercentage;

    // defines what the success transaction composes of
    auto success = [&](string aKey)
    {
	try {
	    connection* tmp = new connection("dbname = SDB user = postgres password = Juni#20 \
		hostaddr = " + host + " port = 5432");

	    string stmt = "INSERT INTO session (ID, DATA) VALUES (" \
			   + aKey + ", '" + dataVal + "' );";

	    work query(*tmp);

	    query.exec(stmt);

	    // having simultaneous readers/modifiers for each transaction
	    simultaneousTasks(8);

	    // doing a read
	    stmt = "SELECT * FROM session WHERE ID = " + aKey + ";";

	    query.exec(stmt);

	    // doing a deletion
	    stmt = "DELETE FROM session WHERE ID = " + aKey + ";";

	    query.exec(stmt);

	    query.commit();

	    tmp->disconnect();

	} catch (const exception &e) {
	    cerr << e.what() << endl;
	    exit(1);
	}
    };

    // defines what the fail transaction composes of
    auto fail = [&] (string aKey)
    {
	try {
	    connection* tmp = new connection("dbname = SDB user = postgres password = Juni#20 \
		hostaddr = " + host + " port = 5432");

	    // performing an insert
	    string stmt = "INSERT INTO session (ID, DATA) VALUES (" \
			   + aKey + ", '" + dataVal + "' );";

	    work query(*tmp);

	    query.exec(stmt);

	    // performing a deletion

	    stmt = "DELETE FROM session WHERE ID = " + aKey + ";";

	    query.exec(stmt);

	    query.commit();

	    tmp->disconnect();
	    
	
	} catch (const exception &e) {
	    cerr << e.what() << endl;
	    exit(1);
	}
    };

    vector<thread> thread_pool;

    int64_t key = 2000000;

    auto start = chrono::high_resolution_clock::now();

    // each thread will either perform a success transaction or a fail transaction
    for (int i = 0; i < n; i++)
    {
	auto aKey = to_string(key);
	
	if (i < numOfSuccess)
	{
	    // doing a success transaction
	    thread_pool.push_back(thread(success, aKey));
	}


	else
	{
	    // doing a fail transaction
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
