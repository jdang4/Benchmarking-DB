#include <iostream>
#include <stdint.h>
#include <ctime>
#include <thread>
#include <vector>
#include <fstream>
#include <chrono>
#include <time.h>
#include "headers/RedisClient.h"
#include "headers/PostgresClient.h"
#include "headers/BenchmarkManager.h"

#define TRIALS 5

using namespace std;

int main() 
{
    int db;
    string host;

    cout << "\nWELCOME TO DB BENCHMARK APPLICATION!\n" << endl;
    cout << "Select the DB to benchmark:" << endl;
    cout << "1  -  Redis" << endl;
    cout << "2  -  PostgreSQL" << endl;

    cout << "\nEnter in DB Num: ";
    cin >> db;

    if (db == 2)
    {
	cout << "\n############################################################################################################################################\n" << endl;
	cout << "HELP GUIDE\n" << endl;
	cout << "To find the IP Address of the PostgreSQL Primary Container: " << endl;
	cout << "\t1) outside of this application, locate the desired container's Container ID by entering 'docker ps' in the terminal" << endl;
	cout << "\t2) copy the Container ID and enter 'docker inspect <insert container id> | grep IPAddress'" << endl;
	cout << "\t3) the resulted address is the address to enter in the below prompt" << endl;
	
	cout << "\nNOTE: at the prompt below, enter 'q' to exit application to follow these instructions (if needed)" << endl;
	cout << "\n############################################################################################################################################" << endl;
	
	cout << "\nEnter in IP Addess of the PostgreSQL Primary Container: ";
	cin >> host;

	if (host == "q")
	{
	    exit(1);
	}
    }

    cout << endl << endl;

    BenchmarkManager* bm = new BenchmarkManager(5);

    bm->selectDB(db, host);
    bm->connect();

    bm->openCSV();
    bm->closeCSV();

    //bm->initializeDB();
    
    bm->getReadOutput();
    bm->disconnect();

    /*
    // adding the column names
    csv << "Name, Time Elapsed (sec), \n";

    cout << "\n=================================================================================================================================================\n" << endl;

    bm->getReadOutput();

    bm->getInsertOutput();

    bm->getUpdateOutput();

    bm->getDeleteOutput();

    cout << "=================================================================================================================================================\n" << endl;

    csv << "\n" << "\n"; 

    csv << "Simultaneous Readers, Time Elapsed (sec), Average Time per Read, \n";

    bm->getSimultaneousReadersOutput(1);
    bm->getSimultaneousReadersOutput(50);
    bm->getSimultaneousReadersOutput(100);
     
    cout << "=================================================================================================================================================\n" << endl;

    csv << "\n" << "\n";

    csv << "Num of Simultaneous Tasks, Time Elapsed (sec), Average Time per Task, \n";

    bm->getSimultaneousTasksOutput(1);
    bm->getSimultaneousTasksOutput(50);
    bm->getSimultaneousTasksOutput(100);
    */

    /*
    cout << "=================================================================================================================================================\n" << endl;

    csv << "\n" << "\n";

    csv << "Num Of Simultaneous Transactions, Time Elapsed (sec), Average Time per Transaction, \n";

    //bm->getTransactionsOutput(1, 70.0);
    //bm->getTransactionsOutput(50, 70.0);
    //bm->getTransactionsOutput(100, 70.0);

    cout << "\n=================================================================================================================================================\n" << endl;
    
    csv.close();
    */
    //client = new PostgresClient("172.17.0.4");
}


