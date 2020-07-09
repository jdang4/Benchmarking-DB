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



void PostgresClient::disconnect()
{
    postgres->disconnect();
    cout << "POSTGRESQL DISCONNECTED\n" << endl;
}


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




double PostgresClient::simultaneousReaders(int n, string key) 
{
    vector<thread> thread_pool;

    auto read = [&](string aKey) {
	try {
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



double PostgresClient::simultaneousTasks(int n)
{
    set<int> keySet = DBClient::getRandomKeys(n, 1, 1000000);

    int randomKeys[n];

    copy(keySet.begin(), keySet.end(), randomKeys);

    if (n <= 0)
    {
	return -1;
    }

    else if (n == 1)
    {
	return readEntry(to_string(randomKeys[0]));
    }

    vector<thread> thread_pool;

    auto read = [&](string aKey) {
	try {
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



double PostgresClient::performTransactions(int n, double successPercentage)
{
    if (successPercentage < 0 || successPercentage > 100 || n <= 0)
    {
	return -1;
    }

    int numOfSuccess = n * successPercentage;

    auto success = [&](string aKey)
    {
	try {
	    connection* tmp = new connection("dbname = SDB user = postgres password = Juni#20 \
		hostaddr = " + host + " port = 5432");

	    string stmt = "INSERT INTO session (ID, DATA) VALUES (" \
			   + aKey + ", '" + dataVal + "' );";

	    work query(*tmp);

	    query.exec(stmt);

	    simultaneousTasks(8);

	    stmt = "SELECT * FROM session WHERE ID = " + aKey + ";";

	    query.exec(stmt);

	    stmt = "DELETE FROM session WHERE ID = " + aKey + ";";

	    query.exec(stmt);

	    query.commit();

	    tmp->disconnect();

	} catch (const exception &e) {
	    cerr << e.what() << endl;
	    exit(1);
	}
    };

    auto fail = [&] (string aKey)
    {
	try {
	    connection* tmp = new connection("dbname = SDB user = postgres password = Juni#20 \
		hostaddr = " + host + " port = 5432");

	    string stmt = "INSERT INTO session (ID, DATA) VALUES (" \
			   + aKey + ", '" + dataVal + "' );";

	    work query(*tmp);

	    query.exec(stmt);

	    simultaneousTasks(8);

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
