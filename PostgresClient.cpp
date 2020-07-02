#include <iostream>
#include <chrono>

#include "headers/PostgresClient.h"

using namespace std;
using namespace pqxx;

PostgresClient::PostgresClient(string host_name) : DBClient() 
{
    dataVal = DBClient::getEntryVal('a');
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
	    cout << "CONNECTED TO POSTGRESQL\n" << endl; 
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

	auto start = chrono::high_resolution_clock::now();

	string stmt = "SELECT * FROM session WHERE ID = " + key + ";";

	nontransaction query(*postgres);

	query.exec(stmt);

	auto end = chrono::high_resolution_clock::now();

	return DBClient::calculateTime(start, end);

    } catch (const exception &e) {
	cerr << e.what() << endl;
    }

    return -1;
}
