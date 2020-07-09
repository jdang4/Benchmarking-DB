#include <iostream>
#include <vector>
#include "headers/BenchmarkManager.h"

using namespace std;

BenchmarkManager::BenchmarkManager(int n)
{
    trials = n;
}

BenchmarkManager::~BenchmarkManager() {}


void BenchmarkManager::selectDB(int db, string host)
{
    dbClient = db;

    switch(db)
    {
	case 1: 
	    client = new RedisClient();
	    break;

	case 2:
	    client = new PostgresClient(host);
	    break;

	default:
	    cout << "INVALID DB CHOICE!" << endl;
	    exit(1);
    }
}

void BenchmarkManager::connect()
{
    client->connect();
    cout << "------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    
}

void BenchmarkManager::disconnect()
{
    client->disconnect();
}

void BenchmarkManager::initializeDB()
{
    double time = client->initializeDB();

    if (time != -1.0)
    {
	cout << "time: " << time << " sec" << endl;
    }

    cout << "\n------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

}

void BenchmarkManager::openCSV()
{
    string file;

    switch(dbClient) 
    {
	case 1 :
	    file = "stats/redis-stats.csv";
	    break;

	case 2 :
	    file = "stats/postgres-stats.csv";  
	    break;

	default :
	    cout << "ERROR FOUND: INVALID DB CLIENT NUMBER" << endl;
	    exit(1);
    }
    
    if (remove(file.c_str()) != 0) 
    {
	cout << "ERROR DELETING CSV FILE" << endl;
	exit(1);
    }

    csv = new ofstream(file);
}


void BenchmarkManager::closeCSV()
{
    csv->close();
}


void BenchmarkManager::getReadOutput()
{
    *csv << "Read Num, Time Elapsed (sec), \n";

    vector<double> times;

    for (int i = 1; i <= trials; i++)
    {
	double time = client->readEntry("506550");

	if (time != -1.0) {
	    cout << "READ #" << i << ":\t\t\t\t\t";
	    cout << "time: " << time << " sec\n" << endl;
	    *csv << i << "," << time << "\n";

	    times.push_back(time);
	}

    }

    double sumTime = 0;

    for (double t : times)
    {
	sumTime += t;
    }

    double averageTime = sumTime / double(times.size());
    
    double avg_op_per_sec = double(times.size()) / sumTime;

    cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Read Average Time: " << averageTime << " sec\t\t Number of Reads: " << trials;
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " reads/sec ]\n" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    *csv << "\n" << "\n";
}



void BenchmarkManager::getInsertOutput()
{
    *csv << "Insert Num, Time Elapased (sec), \n";

    vector<double> times;

    int64_t key = 2000000;

    for (int i = 1; i <= trials; i++)
    {
	double time = client->insertEntry(to_string(key));

	if (time != -1.0)
	{
	    cout << "INSERTION #" << i << ":\t\t\t\t\t";
	    cout << "time: " << time << " sec\n" << endl;
	    *csv << i << "," << time << "\n";

	    times.push_back(time);
	}

	key++;
    }

    double sumTime = 0; 

    for (double t : times)
    {
	sumTime += t;
    }

    double averageTime = sumTime / double(times.size());
    
    double avg_op_per_sec = double(times.size()) / sumTime;

    cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Insert Average Time: " << averageTime << " sec\t\t Number of Inserts: " << trials; 
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " inserts/sec ]\n" << endl; 
    cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    *csv << "\n" << "\n";
   
}



void BenchmarkManager::getUpdateOutput()
{
    *csv << "Update Num, Time Elapsed (sec), \n";

    vector<double> times;
    
    int64_t key = 2000000;

    for (int i = 1; i <= trials; i++)
    {
	double time = client->updateEntry(to_string(key));

	if (time != -1.0)
	{	
	    cout << "UPDATE #" << i << ":\t\t\t\t\t";
	    cout << "time: " << time << " sec\n" << endl;
	    *csv << i << "," << time << "\n";

	    times.push_back(time);
	}

	key++;
    }

    double sumTime = 0;

    for (double t : times)
    {
	sumTime += t;
    }

    double averageTime = sumTime / double(times.size());
    
    double avg_op_per_sec = double(times.size()) / sumTime;

    cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Update Average Time: " << averageTime << " sec\t\t Number of Updates: " << trials;
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " updates/sec ]\n" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    *csv << "\n" << "\n";

}



void BenchmarkManager::getDeleteOutput()
{
    *csv << "Delete Num, Time Elapsed (sec), \n";
    vector<double> times;

    int64_t key = 2000000;

    for (int i = 1; i <= trials; i++) 
    {
	double time = client->deleteEntry(to_string(key));

	if (time != -1.0) 
	{
	    cout << "DELETION #" << i << ":\t\t\t\t\t"; 
	    cout << "time: " << time << " sec\n" << endl;
	    *csv << i << "," << time << "\n";
	    
	    times.push_back(time);
	}

	key++;
    }

    double sumTime = 0;

    for (double t : times)
    {
	sumTime += t;
    }

    double averageTime = sumTime / double(times.size());
    
    double avg_op_per_sec = double(times.size()) / sumTime;

    cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Deletion Average Time: " << averageTime << " sec\t\t Number of Deletions: " << trials; 
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " deletions/sec ]\n" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    *csv << "\n" << "\n";

}



void BenchmarkManager::getSimultaneousReadersOutput(int n)
{
    *csv << n << " Simul. Reader Num, Time Elapsed (sec), Average Time per Read, \n";

    vector<double> times;

    double sumRead_per_sec = 0;

    for (int i = 1; i <= trials; i++)
    {
	double time = client->simultaneousReaders(n, "550500");
	double avgTime_per_read = time / double(n);
	double avg = double(n) / time;

	cout << n << " Simultaneous Readers #" << i << ":\t\t\t";
	cout << "time: " << time << " sec\t\t";
	cout << "average time per read: " << avgTime_per_read << " sec\n" << endl;
	*csv << i << "," << time << "," << avgTime_per_read << "\n";

	sumRead_per_sec += avg;

	times.push_back(time);
    }

    double sumTime = 0;

    for (double t : times)
    {
	sumTime += t;
    }

    double averageTime = sumTime / double(times.size());
    
    double avg_op_per_sec = double(n * trials) / sumTime; 

    cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Simul. Read Average Time: " << averageTime << " sec\t\t Number of Trials: " << trials; 
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " reads/sec ]\n" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    *csv << "\n" << "\n";
}



void BenchmarkManager::getSimultaneousTasksOutput(int n)
{
    *csv << n << " Simul. Tasks Num, Time Elapsed (sec), Average Time per Tasks, \n";

    vector<double> times;

    for (int i = 1; i <= trials; i++)
    {
	double time = client->simultaneousTasks(n);
	double avgTime_per_task = time / double(n);

	cout << n << " Simultaneous Tasks #" << i << ":\t\t\t";
	cout << "time: " << time << " sec\t\t";
	cout << "average time per task: " << avgTime_per_task << " sec\n" << endl;
	*csv << i << "," << time << "," << avgTime_per_task << "\n";

	times.push_back(time);
    }

    double sumTime = 0;

    for (double t : times)
    {
	sumTime += t;
    }

    double averageTime = sumTime / double(times.size());
    
    double avg_op_per_sec = double(n * trials) / sumTime; 

    cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Simul. Tasks Average Time: " << averageTime << " sec\t\t Number of Trials: " << trials; 
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " tasks/sec ]\n" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    *csv << "\n" << "\n";
}



void BenchmarkManager::getTransactionsOutput(int n, double successPercentage)
{
    *csv << n << " Simul. Transactions, Time Elapsed (sec), Average Time per Tasks, \n";

    vector<double> times;

    for (int i = 1; i <= trials; i++)
    {
	double time = client->performTransactions(n, successPercentage);
	double avgTime_per_transaction = time / double(n);

	cout << n << " Simultaneous Transactions #" << i << ":\t\t\t";
	cout << "time: " << time << " sec\t\t";
	cout << "average time per transaction: " << avgTime_per_transaction << " sec\n" << endl;
	*csv << i << "," << time << "," << avgTime_per_transaction << "\n";

	times.push_back(time);
    }

    double sumTime = 0;

    for (double t : times)
    {
	sumTime += t;
    }
    
    double averageTime = sumTime / double(times.size());
    
    double avg_op_per_sec = double(n * trials) / sumTime; 

    cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Simul. Transactions Average Time: " << averageTime << " sec\t\t Number of Trials: " << trials; 
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " transactions/sec ]\n" << endl;
    cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    *csv << "\n" << "\n";
}
