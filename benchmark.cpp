#include <iostream>
#include "headers/RedisClient.h"
#include "headers/PostgresClient.h"
#include "headers/BenchmarkManager.h"
#include <chrono>
#include <unistd.h>

using namespace std;

int main() 
{
    int db, numOfRuns;
    string host, initialize_option, print_option;

    bool initialize_db = false;
    bool printOutputs = true;

    double durationTime = 1;

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

    cout << endl;

    cout << "Do you want to show all the print outputs? [y/n]: ";
    cin >> print_option;

    if (print_option == "n")
    {
	printOutputs = false;
    }

    cout << endl;

    cout << "Select how long (in seconds) you want program to last:" << endl;
    cout << "NOTE: roughly less than 1 second, the program will run once\n" << endl;
    
    cout << "Enter time (in seconds): ";
    cin >> durationTime;

    cout << endl;

    cout << "Select the number of operations to perform for each benchmark test:" << endl;
    cout << "1	-   Each operation is performed on 1 entry" << endl;
    cout << "2	-   Each operation is performed on 500 entries" << endl;
    cout << "3	-   Each operation is performed on 5,000 entries" << endl;
    cout << "4	-   Each operation is performed on 500,000 entries" << endl;
    cout << "5	-   Each operation is performed on 1,000,000 entries" << endl;

    cout << "\nEnter in a number listed above: ";
    cin >> numOfRuns;

    switch(numOfRuns)
    {
	case 1:
	    numOfRuns = 1;
	    break;

	case 2:
	    numOfRuns = 500;
	    break;

	case 3:
	    numOfRuns = 5000;
	    break;

	case 4:
	    numOfRuns = 500000;
	    break;

	case 5:
	    numOfRuns = 1000000;
	    break;

	default:
	    cout << "INVALID ENTRY!" << endl;
	    exit(-1);
    }


    durationTime = (durationTime > 0) ? durationTime : 1;

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


    BenchmarkManager* bm = new BenchmarkManager(3, printOutputs);

    string file = (db == 1) ? "stats/" + to_string(numOfRuns) + "/redis-running-stats.csv" : "stats/" + to_string(numOfRuns) + "/postgres-running-stats.csv";

    if (remove(file.c_str()) != 0)
    {
	cout << "ERROR DELETING RUNNING STATS CSV FILE" << endl;
	exit(1);
    }

    ofstream csv(file);

    bm->selectDB(db, host);
    bm->setThreads_and_Runs(10, 1000000);
    bm->connect();

    if (initialize_db)
    {
	bm->initializeDB();
    }

    auto start = chrono::high_resolution_clock::now();

    double elapsedTime = 0;

    csv << "Timestamp,Reading,Inserting,Updating,Deleting,"; 
    //csv << "1 Simul. Reader,50 Simul. Reader,100 Simul. Reader,";
    csv << "10 Simul. Tasks,50 Simul. Task,100 Simul. Task,";
    csv << "10 Simul. Transactions,50 Simul. Transaction,100 Simul. Transaction\n";

    double read = 0, insert = 0, update = 0, deletion = 0;
    //double reader_1 = 0, reader_50 = 0, reader_100 = 0;
    double task_1 = 0, task_50 = 0, task_100 = 0;
    double transaction_1 = 0, transaction_50 = 0, transaction_100 = 0;

    while (elapsedTime < durationTime)
    {
	bm->openCSV(numOfRuns);

	bm->setThreads_and_Runs(10, numOfRuns);

	cout << "READING BENCHMARK: \n\n\n\n" << endl;

	read = bm->getReadOutput();

	cout << "INSERTION BENCHMARK: \n\n\n\n" << endl; 
    
	insert = bm->getInsertOutput();

	cout << "UPDATING BENCHMARK: \n\n\n\n" << endl;

	update = bm->getUpdateOutput(); 

	cout << "DELETION BENCHMARK: \n\n\n\n" << endl;

	deletion = bm->getDeleteOutput();
	
	cout << "SIMULTANEOUS TASKS [10] BENCHMARK: \n\n\n\n" << endl;

	bm->setThreads_and_Runs(10, numOfRuns);

	task_1 = bm->getSimultaneousTasksOutput(1);

	cout << "SIMULTANEOUS TASKS [50] BENCHMARK: \n\n\n\n" << endl;

	bm->setThreads_and_Runs(50, numOfRuns);
	
	task_50 = bm->getSimultaneousTasksOutput(50);

	cout << "SIMULTANEOUS TASKS [100] BENCHMARK: \n\n\n\n" << endl;

	bm->setThreads_and_Runs(100, numOfRuns);
	
	task_100 = bm->getSimultaneousTasksOutput(50);

	cout << "SIMULTANEOUS TRANSACTIONS [10] BENCHMARK: \n\n\n\n" << endl;

	bm->setThreads_and_Runs(10, numOfRuns);

	transaction_1 = bm->getTransactionsOutput(1, 70.0);

	cout << "SIMULTANEOUS TRANSACTIONS [50] BENCHMARK: \n\n\n\n" << endl;

	bm->setThreads_and_Runs(50, numOfRuns);
	
	transaction_50 = bm->getTransactionsOutput(50, 70.0);

	cout << "SIMULTANEOUS TRANSACTIONS [100] BENCHMARK: \n\n\n\n" << endl;

	bm->setThreads_and_Runs(100, numOfRuns);
	
	transaction_100 = bm->getTransactionsOutput(100, 70.0);


	auto end = chrono::high_resolution_clock::now();

	auto elapsed = chrono::duration_cast<chrono::microseconds>(end - start);

	elapsedTime = elapsed.count() * 1e-6;

	double elapsedTime_min = double(elapsedTime) / double(60);

	
	csv << elapsedTime_min << "," << read << "," << insert << "," << update << "," << deletion << ",";
	//csv << reader_1 << "," << reader_50 << "," << reader_100 << ",";
	csv << task_1 << "," << task_50 << "," << task_100 << ",";
	csv << transaction_1 << "," << transaction_50 << "," << transaction_100 << "\n";
	
	bm->closeCSV();
    }


    csv.close();

    bm->disconnect();

}


