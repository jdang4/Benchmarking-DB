#include <iostream>
#include "headers/RedisClient.h"
#include "headers/PostgresClient.h"
#include "headers/BenchmarkManager.h"

using namespace std;

int main() 
{
    int db;
    string host;
    string initialize_option;
    bool initialize_db = false;

    cout << "\nWELCOME TO DB BENCHMARK APPLICATION!\n" << endl;
    cout << "Select the DB to benchmark:" << endl;
    cout << "1  -  Redis" << endl;
    cout << "2  -  PostgreSQL" << endl;

    cout << "\nEnter in DB Num: ";
    cin >> db;

    cout << endl;

    cout << "Do you want to initialize the DB? [y/n]: ";
    cin >> initialize_option;

    if (initialize_option == "y") 
    {
	initialize_db = true;
    }

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

    if (initialize_db)
    {
	bm->initializeDB();
    }
    
    cout << "READING 1 KEY BENCHMARK: \n\n\n\n" << endl;

    bm->getReadOutput();
    
    cout << "INSERTING 1 KEY BENCHMARK: \n\n\n\n" << endl; 
    
    bm->getInsertOutput();

    cout << "UPDATING 1 KEY BENCHMARK: \n\n\n\n" << endl;

    bm->getUpdateOutput(); 

    cout << "DELETING 1 KEY BENCHMARK: \n\n\n\n" << endl;

    bm->getDeleteOutput();

    cout << "SIMULTANEOUS READERS [1] BENCHMARK: \n\n\n\n" << endl;

    bm->getSimultaneousReadersOutput(1);
    
    cout << "SIMULTANEOUS READERS [50] BENCHMARK: \n\n\n\n" << endl;
    
    bm->getSimultaneousReadersOutput(50);
    
    cout << "SIMULTANEOUS READERS [100] BENCHMARK: \n\n\n\n" << endl;
     
    bm->getSimultaneousReadersOutput(100);

    cout << "SIMULTANEOUS TASKS [1] BENCHMARK: \n\n\n\n" << endl;

    bm->getSimultaneousTasksOutput(1);

    cout << "SIMULTANEOUS TASKS [50] BENCHMARK: \n\n\n\n" << endl;

    bm->getSimultaneousTasksOutput(50);

    cout << "SIMULTANEOUS TASKS [100] BENCHMARK: \n\n\n\n" << endl;

    bm->getSimultaneousTasksOutput(100);

    cout << "SIMULTANEOUS TRANSACTIONS [1] BENCHMARK: \n\n\n\n" << endl;

    bm->getTransactionsOutput(1, 70.0);

    cout << "SIMULTANEOUS TRANSACTIONS [50] BENCHMARK: \n\n\n\n" << endl;

    bm->getTransactionsOutput(50, 70.0);

    cout << "SIMULTANEOUS TRANSACTIONS [100] BENCHMARK:	\n\n\n\n" << endl;

    bm->getTransactionsOutput(100, 70.0);

    /*
    for (int i = 10; i <= 100; i += 10)
    {
	bm->getSimultaneousReadersOutput(i, i);
    }

    for (int i = 10; i <= 100; i += 10) 
    {
	bm->getSimultaneousTasksOutput(i, i);
    }

    for (int i = 10; i <= 100; i += 10)
    {
	bm->getTransactionsOutput(i, 70.0, i);
    }
    */

    bm->closeCSV();

    bm->disconnect();

}


