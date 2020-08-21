#include <iostream>
#include "headers/RedisClient.h"
#include "headers/PostgresClient.h"
#include "headers/BenchmarkManager.h"
#include <chrono>
#include <unistd.h>
#include <fstream>
 
using namespace std;
 
int main() 
{
    int db, numOfEntries, benchmarkOption, recordSize;
    string host, initialize_option, print_option;
 
    bool printOutputs = true;
    bool randomizeKey = false;
 
    double durationTime = 1;
 
    cout << "\nWELCOME TO DB BENCHMARK APPLICATION!\n" << endl;
    cout << "Select the DB to benchmark:" << endl;
    cout << "1  -  Redis" << endl;
    cout << "2  -  PostgreSQL" << endl; 
    cout << "\nEnter in DB Num: ";
    cin >> db;
 
    cout << endl;

   /* 
    cout << "Do you want to initialize the DB? [y/n]: ";
    cin >> initialize_option;
 
    if (initialize_option == "y") 
    {
    	initialize_db = true;
    }
 
    cout << endl;
    */

    cout << "Do you want to show all the print outputs? [y/n]: ";
    cin >> print_option;
 
    if (print_option == "n")
    {
    	printOutputs = false;
    }
 
    cout << endl;
 
    cout << "Select how long (in seconds) you want program to last:" << endl;
    cout << "NOTE: roughly less than 1 second would cause the program will run in one round. Refer to README for additional details \n" << endl;
    
    cout << "Enter time (in seconds): ";
    cin >> durationTime;

    // error checking
    durationTime = (durationTime > 0) ? durationTime : 1;
 
    cout << endl;

    cout << "Select the option you want to use in the benchmark application:" << endl;
    cout << "1  -  (Standard) Already defined set of benchmark tests" << endl;
    cout << "2  -  (Standard) Already defined set of benchmark tests with Randomize Keys" << endl;
    cout << "3  -  Randomize Operations and Keys" << endl;
    
    cout << "\nEnter in option: ";
    cin >> benchmarkOption;

    if (benchmarkOption == 2)
    {
        randomizeKey = true;
    }

    cout << endl;

    cout << "Select the number of operations to perform for each benchmark test:" << endl;
    cout << "1  -   Each operation is performed on 1 entry" << endl;
    cout << "2  -   Each operation is performed on 500 entries" << endl;
    cout << "3  -   Each operation is performed on 5,000 entries" << endl;
    cout << "4  -   Each operation is performed on 500,000 entries" << endl;
    cout << "5  -   Each operation is performed on 1,000,000 entries" << endl;
 
    cout << "\nEnter in a number listed above: ";
    cin >> numOfEntries;
 
    switch(numOfEntries)
    {
    	case 1:
        	numOfEntries = 1;
        	break;
 
    	case 2:
        	numOfEntries = 500;
        	break;
 
    	case 3:
        	numOfEntries = 5000;
        	break;
 
	    case 4:
		    numOfEntries = 500000;
        	break;
 
	    case 5:
		    numOfEntries = 1000000;
        	break;
 
	    default:
		    cout << "INVALID ENTRY!" << endl;
        	exit(-1);
    }

    cout << endl;

    cout << "Select which record size to use during the benchmark test:" << endl;
    cout << "1  -   Record Size is 256 bytes" << endl;
    cout << "2  -   Record Size is 2K bytes" << endl;
    cout << "3  -   Record Size is 10K bytes" << endl;
    cout << "4  -   Record Size is 6K bytes (default)" << endl;
 
    cout << "\nEnter in a number listed above: ";
    cin >> recordSize;

    switch (recordSize)
    {
        case 1 :
            recordSize = 256;
            //initialize_db = true;
            break;

        case 2 :
            recordSize = 2000;
            //initialize_db = true;
            break;

        case 3 :
            recordSize = 10000;
            //initialize_db = true;
            break;

        default :
            recordSize = 6000;
            break;
    }

    cout << "\n############################################################################################################################################\n" << endl;
    cout << "HELP GUIDE\n"<< endl;
    cout << "To find the IP Address of the PostgreSQL Primary Container: " << endl;
    cout << "\t1) outside of this application, locate the desired container's Container ID by entering 'docker ps' in the terminal" << endl;
    cout << "\t2) copy the Container ID and enter 'docker inspect <insert container id> | grep IPAddress'" << endl;
    cout << "\t3) the resulted address is the address to enter in the below prompt" << endl;

    cout << "\nNOTE: at the prompt below, enter 'q' to exit application to follow these instructions (if needed)" << endl;
    cout << "\n############################################################################################################################################" << endl;

    cout << "\nEnter in IP Addess of the Primary Container: ";
    cin >> host;

    if (host == "q")
    {
        exit(1);
    }

    cout << endl << endl;

   int trials = 1; 
 
    BenchmarkManager* bm = new BenchmarkManager(trials, printOutputs);
 
    int incrementor = trials * 1000000;
 
    int start_1 = 2000000;
 
    int start_5 = start_1 + incrementor;

    int start_10 = start_5 + incrementor;

    int start_10_transaction = start_10 + incrementor;

    int start_50 = start_10_transaction + incrementor;

    int start_100 = start_50 + incrementor;

    // creating the file path based on the selected DB
    string file = (db == 1) ? "stats/" + to_string(recordSize) + "/" + to_string(numOfEntries) + "/redis-running-stats.csv" : "stats/" + to_string(recordSize) + "/" + to_string(numOfEntries) + "/postgres-running-stats.csv";
 
    if (remove(file.c_str()) != 0)
    {
        cout << "ERROR DELETING RUNNING STATS CSV FILE" << endl;
        exit(1);
    }

    ifstream stats_file;

    stats_file.open(file);

    if (!stats_file)
    {
        ofstream create_file {file};
    }
    
    ofstream csv(file);

    bm->selectDB(db, host, recordSize);
    bm->setThreads_and_Entries(10, 1000000);
    bm->connect();
 
    bm->initializeDB();
 
    auto start = chrono::high_resolution_clock::now();
 
    double elapsedTime = 0;

    // this csv file is for the one with running in the filename
    csv << "Timestamp,Reading 1,Reading 5, Reading 10, ";
    csv << "Inserting 1,Inserting 5,Inserting 10, "; 
    csv << "Updating 1,Updating 5,Updating 10, ";
    csv << "Deleting 1,Deleting 5,Deleting  10, "; 
    csv << "10 Simul. Readers,50 Simul. Reader,100 Simul. Readers,";
    csv << "10 Simul. Tasks,50 Simul. Tasks,100 Simul. Tasks,";
    csv << "10 Simul. Transactions,50 Simul. Transactions,100 Simul. Transactions\n";
 
    double read_1 = 0, read_5 = 0, read_10 = 0; 
    double insert_1 = 0, insert_5 = 0, insert_10 = 0; 
    double update_1 = 0, update_5 = 0, update_10 = 0;
    double delete_1 = 0, delete_5 = 0, delete_10 = 0;
    double task_10 = 0, task_50 = 0, task_100 = 0;
    double transaction_10 = 0, transaction_50 = 0, transaction_100 = 0;

    // using while loop to allow program to run for at least the amount of time the user wants it to run, by running the program
    // in multiple rounds (if needed)
    while (elapsedTime < durationTime)
    {
        // performing the standard tests
		if (benchmarkOption != 3)
		{
		    bm->openCSV(numOfEntries, recordSize);
 
		    cout << "READING BENCHMARK: \n\n" << endl;
 
		    cout << "1 THREAD: \n\n" << endl;
 
		    read_1 = bm->getReadOutput(1, true, randomizeKey);
 
		    cout << "5 THREAD: \n\n" << endl;
 
		    read_5 = bm->getReadOutput(5, true, randomizeKey);
 
		    cout << "10 THREAD: \n\n" << endl;
 
		    read_10 = bm->getReadOutput(10, true, randomizeKey);
 
		    cout << "INSERTION BENCHMARK: \n\n" << endl; 
 
		    cout << "1 THREAD: \n\n" << endl;
 
		    insert_1 = bm->getInsertOutput(1, start_1, true);
 
		    cout << "5 THREAD: \n\n" << endl;
 
		    insert_5 = bm->getInsertOutput(5, start_5, true);
 
		    cout << "10 THREAD: \n\n" << endl;
 
		    insert_10 = bm->getInsertOutput(10, start_10, true);

		    cout << "UPDATING BENCHMARK: \n\n" << endl;
 
		    cout << "1 THREAD: \n\n" << endl;
 
		    update_1 = bm->getUpdateOutput(1, start_1, true, randomizeKey); 
 
		    cout << "5 THREAD: \n\n" << endl;
        
		    update_5 = bm->getUpdateOutput(5, start_5, true, randomizeKey);
 
		    cout << "10 THREAD: \n\n" << endl;
        
		    update_10 = bm->getUpdateOutput(10, start_10, true, randomizeKey);

		    cout << "DELETION BENCHMARK: \n\n" << endl;
 
		    cout << "1 THREAD: \n\n" << endl;
 
		    delete_1 = bm->getDeleteOutput(1, start_1, true, randomizeKey);
 
		    cout << "5 THREAD: \n\n" << endl;
        
		    delete_5 = bm->getDeleteOutput(5, start_5, true, randomizeKey);
 
		    cout << "10 THREAD: \n\n" << endl;
 
		    delete_10 = bm->getDeleteOutput(10, start_10, true, randomizeKey);

		    cout << "SIMULTANEOUS TASKS [10] BENCHMARK: \n\n\n\n" << endl;
 
		    task_10 = bm->getSimultaneousTasksOutput(10, true, randomizeKey);
 
		    cout << "SIMULTANEOUS TASKS [50] BENCHMARK: \n\n\n\n" << endl;
    
		    task_50 = bm->getSimultaneousTasksOutput(50, true, randomizeKey);
 
		    cout << "SIMULTANEOUS TASKS [100] BENCHMARK: \n\n\n\n" << endl;
    
		    task_100 = bm->getSimultaneousTasksOutput(100, true, randomizeKey);

		    cout << "SIMULTANEOUS TRANSACTIONS [10] BENCHMARK: \n\n\n\n" << endl;

		    transaction_10 = bm->getTransactionsOutput(10, start_10_transaction, true);
 
		    cout << "SIMULTANEOUS TRANSACTIONS [50] BENCHMARK: \n\n\n\n" << endl;
    
		    transaction_50 = bm->getTransactionsOutput(50, start_50, true);
 
		    cout << "SIMULTANEOUS TRANSACTIONS [100] BENCHMARK: \n\n\n\n" << endl;
    
		    transaction_100 = bm->getTransactionsOutput(100, start_100, true);

		    auto tmp_end = chrono::high_resolution_clock::now();

		    auto tmp_elapsed = chrono::duration_cast<chrono::microseconds>(tmp_end - start);

		    elapsedTime = tmp_elapsed.count() * 1e-6;

		    double elapsedTime_min = double(elapsedTime) / double(60);

            // recording in the CSV file with running in the filename
		    csv << elapsedTime_min << "," << read_1 << "," << read_5 << "," << read_10 << ",";
		    csv << insert_1 << "," << insert_5 << "," << insert_10 << ","; 
		    csv << update_1 << "," << update_5 << "," << update_10 << ","; 
		    csv << delete_1 << "," << delete_5 << "," << delete_10 << ",";
		    csv << task_10 << "," << task_50 << "," << task_100 << ",";
		    csv << transaction_10 << "," << transaction_50 << "," << transaction_100 << "\n";

		    bm->closeCSV();
		}

        // perform the complete randomization test
		else 
		{
		    bm->performRandomization();
		}   
 
 
		auto end = chrono::high_resolution_clock::now();
 
		auto elapsed = chrono::duration_cast<chrono::microseconds>(end - start);
 
		elapsedTime = elapsed.count() * 1e-6;
	}
	
 
    csv.close();

    bm->disconnect();
 
}
 
 
 

