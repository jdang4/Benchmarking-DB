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
    vector<double> times;

    DBClient* client = new RedisClient(true, true);
    
    BenchmarkManager* bm = new BenchmarkManager(5);

    bm->selectDB(1, "");

    bm->openCSV();
    bm->closeCSV();

    //double time= client->initializeDB();
    double time = -1.0;

    if (time != -1.0)
    {
	cout << "time: " << time << " sec\n" << endl;
    }

    if (remove("stats.csv") != 0)
    {
	cout << "ERROR DELETING benchmark-stats file";
    }

    ofstream csv("stats.csv");

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

    /*
    time = client->simultaneousTasks(1);

    cout << "1 Task:\t\t\t\t\t\t";
    cout << "time: " << time << " sec\n" << endl;
    csv << 1 << ", " << time << ", , \n";

    time = client->simultaneousTasks(50);
    double avgTime = double(time) / double(50);

    cout << "50 Simultaneous Tasks:\t\t\t\t";
    cout << "time: " << time << " sec\t\t";
    cout << "average time per task: " << avgTime << " sec\n" << endl;
    csv << 50 << ", " << time << ", " << avgTime << ", \n";
    
    time = client->simultaneousTasks(100);
    avgTime = double(time) / double(100);

    cout << "100 Simultaneous Tasks:\t\t\t\t";
    cout << "time: " << time << " sec\t\t";
    cout << "average time per task: " << avgTime << " sec\n" << endl;
    csv << 100 << ", " << time << ", " << avgTime << ", \n";
    */
    /*
    cout << "=================================================================================================================================================\n" << endl;

    csv << "\n" << "\n";

    csv << "Num of Simultaneous Tasks, Time Elapsed (sec), Average Time per Task, \n";
     
    for (int i = 10; i <= 200; i += 10)
    {
	time = client->simultaneousTasks(i);
	avgTime = double(time) / double(i);

	cout << i << " Simultaneous Tasks:\t\t\t\t";
	cout << "time: " << time << " sec\t\t";
	cout << "average time per task: " << double(time) / double(i) << " sec\n" << endl;
	csv << i << ", " << time << ", " << avgTime << ", \n";
    }
    */

    cout << "=================================================================================================================================================\n" << endl;

    csv << "\n" << "\n";

    csv << "Num Of Simultaneous Transactions, Time Elapsed (sec), Average Time per Transaction, \n";

    bm->getTransactionsOutput(1, 70.0);
    bm->getTransactionsOutput(50, 70.0);
    bm->getTransactionsOutput(100, 70.0);

    /*
    time = client->performTransactions(1, 70.0);

    cout << "1 Transaction:\t\t\t\t\t";
    cout << "time: " << time << " sec\n" << endl;
    csv << 1 << ", " << time << ", , \n";


    time = client->performTransactions(50, 70.0);
    double avgTime = double(time) / double(50);

    cout << "50 Transactions:\t\t\t\t";
    cout << "time: " << time << " sec\t\t";
    cout << "average time per transaction: " << avgTime << ", \n" << endl;
    csv << 50 << ", " << time << ", " << avgTime << ", \n";


    time = client->performTransactions(100, 70.0);
    avgTime = double(time) / double(100);

    cout << "100 Transactions:\t\t\t\t";
    cout << "time: " << time << " sec\t\t";
    cout << "average time per transaction: " << avgTime << ", \n" << endl;
    csv << 100 << ", " << time << ", " << avgTime << ", \n";
    */
    cout << "\n=================================================================================================================================================\n" << endl;
    
    csv.close();

    client = new PostgresClient("172.17.0.4");
}


