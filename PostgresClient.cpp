#include <iostream>
#include "headers/PostgresClient.h"

using namespace std;
using namespace pqxx;

PostgresClient::PostgresClient(string host) : DBClient() 
{
    entryVal = DBClient::getEntryVal('a');

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

PostgresClient::~PostgresClient() {}
