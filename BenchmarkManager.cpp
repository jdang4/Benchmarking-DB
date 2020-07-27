#include <iostream>
#include <vector>
#include "headers/BenchmarkManager.h"
 
using namespace std;
 
BenchmarkManager::BenchmarkManager(int n, bool option)
{
    trials = n;
    showOutputs = option;
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
 
void BenchmarkManager::setThreads_and_Runs(int threads, int runs)
{
    client->setThreads(threads);
    client->setRuns(runs);
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
 
void BenchmarkManager::openCSV(int n)
{
    string file;
 
    switch(dbClient) 
    {
    case 1 :
        file = "stats/" + to_string(n) + "/redis-stats.csv";
        break;
 
    case 2 :
        file = "stats/" + to_string(n) + "/postgres-stats.csv";  
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


void BenchmarkManager::performRandomization()
{
    int option = (rand() % 6) + 1;

    int starting_key = (rand() % (3000000 + 1 - 2000000)) + 2000000;

    int thread_counts[3] = {1, 5, 10};

    int index = (rand() % (3 + 1));

    index--;

    int thread = thread_counts[index];

    string title;

    trials = 1;

    switch(option)
    {
        case 1 :
            title = "READING";
            cout << title << " BENCHMARK: \n\n" << endl;
            cout << thread << " THREAD: \n\n" << endl;
            getReadOutput(thread, false, true);
            break;

        case 2 :
            title = "INSERTION";
            cout << title << " BENCHMARK: \n\n" << endl;
            cout << thread << " THREAD: \n\n" << endl;
            getInsertOutput(thread, starting_key, false);
            getDeleteOutput(thread, starting_key, false, false);
            break;

        case 3 :
            title = "UPDATING";
            cout << title << " BENCHMARK: \n\n" << endl;
            cout << thread << " THREAD: \n\n" << endl;
            getUpdateOutput(thread, 1, false, true);
            break;

        case 4 :
            title = "DELETION";
            cout << title << " BENCHMARK: \n\n" << endl;
            cout << thread << " THREAD: \n\n" << endl;
            getDeleteOutput(thread, 1, false, true);
            break;

        case 5 :
            title = to_string(thread) + " SIMULTANEOUS TASKS";
            cout << title << " BENCHMARK: \n\n" << endl;
            cout << thread << " THREAD: \n\n" << endl;
            getSimultaneousTasksOutput(thread, starting_key, false, true);
            break;

        case 6 :
            title = to_string(thread) + " SIMULTANEOUS TRANSACTIONS";
            cout << title << " BENCHMARK: \n\n" << endl;
            cout << thread << " THREAD: \n\n" << endl;
            getTransactionsOutput(thread, starting_key, false, true);
            break;

        default :
            cout << "ERROR" << endl;
            exit(-1);
    } 
}
 
double BenchmarkManager::getReadOutput(int threads, bool csvOption, bool random)
{
    if (csvOption)
    {
        *csv << "Read Num,Threads,Runs,Time Elapsed (sec)\n";
    }
    
 
    vector<double> times;
 
    client->setThreads(threads);
 
    for (int i = 1; i <= trials; i++)
    {
        double time = client->readEntry(random);
 
        if (showOutputs) 
        {
            cout << "READ #" << i << ":\t\t\t\t\t";
            cout << "time: " << time << " sec\n" << endl;
        }

        if (csvOption)
        {
             *csv << i << "," << client->getThreads() << "," << client->getRuns() << "," << time << "\n";
        }
       
 
        times.push_back(time);
 
    }
 
    double sumTime = 0;
 
    for (double t : times)
    {
        sumTime += t;
    }
 
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getRuns()) / sumTime;
 
    cout << "\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Read Average Time: " << averageTime << " sec\t\t Number of Threads: " << client->getThreads() << "\t\t Number of Trials: " << trials;
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " reads/sec ]\n" << endl;
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    if (csvOption)
    {
        *csv << "\n" << "\n";
    }
    
 
    return averageTime;
}
 
 
 
double BenchmarkManager::getInsertOutput(int threads, int start, bool csvOption)
{
    if (csvOption)
    {
        *csv << "Insert Num,Threads,Runs,Time Elapased (sec)\n";
    }
    
 
    vector<double> times;
 
    client->setThreads(threads);
 
    int64_t key = start;
 
    for (int i = 1; i <= trials; i++)
    {
        double time = client->insertEntry(key);
 
        key += 1000000;
 
        if (showOutputs)
        {
            cout << "INSERTION #" << i << ":\t\t\t\t\t";
            cout << "time: " << time << " sec\n" << endl;
        }

        if (csvOption)
        {
            *csv << i << "," << client->getThreads() << "," << client->getRuns() << "," << time << "\n";
        }
        
 
        times.push_back(time);
 
    }
 
    double sumTime = 0; 
 
    for (double t : times)
    {
        sumTime += t;
    }
 
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getRuns()) / sumTime;
 
    cout << "\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Insert Average Time: " << averageTime << " sec\t\t Number of Threads: " << client->getThreads() << "\t\t Number of Trials: " << trials; 
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " inserts/sec ]\n" << endl; 
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    if (csvOption)
    {
        *csv << "\n" << "\n";
    }
    
 
    return averageTime;
}
 
 
 
double BenchmarkManager::getUpdateOutput(int threads, int start, bool csvOption, bool random)
{
    if (csvOption)
    {
        *csv << "Update Num,Threads,Runs,Time Elapsed (sec)\n";
    }
    
 
    vector<double> times;
 
    client->setThreads(threads);
 
    int64_t key = start;
 
    for (int i = 1; i <= trials; i++)
    {
        double time = client->updateEntry(key, random);
 
        key += 1000000;
 
        if (showOutputs)
        {
            cout << "UPDATE #" << i << ":\t\t\t\t\t";
            cout << "time: " << time << " sec\n" << endl;
        }

        if (csvOption)
        {
            *csv << i << "," << client->getThreads() << "," << client->getRuns() << "," << time << "\n";
        }
        
 
        times.push_back(time);
 
    }
 
    double sumTime = 0;
 
    for (double t : times)
    {
        sumTime += t;
    }
 
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getRuns()) / sumTime;
 
    cout << "\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Update Average Time: " << averageTime << " sec\t\t Number of Threads: " << client->getThreads() << "\t\t Number of Trials: " << trials;
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " updates/sec ]\n" << endl;
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    if (csvOption)
    {
        *csv << "\n" << "\n";
    }
    
    
    return averageTime;
}
 
 
 
double BenchmarkManager::getDeleteOutput(int threads, int start, bool csvOption, bool random)
{
    if (csvOption)
    {
        *csv << "Delete Num,Threads,Runs,Time Elapsed (sec)\n";
    }
    
    vector<double> times;
 
    client->setThreads(threads);
 
    int64_t key = start;
 
    for (int i = 1; i <= trials; i++) 
    {
    
        double time = client->deleteEntry(key, random);
 
        key += 1000000;
 
        if (showOutputs)
        {
            cout << "DELETION #" << i << ":\t\t\t\t\t"; 
            cout << "time: " << time << " sec\n" << endl;
        }

        if (csvOption)
        {
            *csv << i << "," << client->getThreads() << "," << client->getRuns() << "," << time << "\n";
        }
        
        
        times.push_back(time);
    }
 
    double sumTime = 0;
 
    for (double t : times)
    {
        sumTime += t;
    }
 
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getRuns()) / sumTime;
 
    cout << "\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Deletion Average Time: " << averageTime << " sec\t\t Number of Threads: " << client->getThreads() << "\t\t Number of Trials: " << trials; 
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " deletions/sec ]\n" << endl;
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    if (csvOption)
    {
        *csv << "\n" << "\n";
    }
    
    
    return averageTime;
}
 
 
double BenchmarkManager::getSimultaneousTasksOutput(int threads, bool csvOption, bool random, int num)
{
    if (num <= 10 && csvOption)
    {
        *csv << threads << " Simul. Tasks Num,Threads,Runs,Time Elapsed (sec),Average Time per Tasks\n";
    }
 
    vector<double> times;
 
    client->setThreads(threads);
 
    int max = (num == 0) ? trials : 1;
 
    for (int i = 1; i <= max; i++)
    {
        double time = client->simultaneousTasks(random);
 
        if (num == 0)
        {
            if (showOutputs)
            {
                cout << threads << " Simultaneous Tasks #" << i << ":\t\t\t";
                cout << "time: " << time << " sec\n" << endl;
            }
        }
    
        i = (num == 0) ? i : num;

        if (csvOption)
        {
            *csv << i << "," << client->getThreads() << "," << client->getRuns() << "," << time << "\n";
        }
 
        times.push_back(time);
    }
 
    double sumTime = 0;
 
    for (double t : times)
    {
        sumTime += t;
    }
 
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getRuns()) / sumTime; 
 
    if (num == 0)
    {
        cout << "\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
        cout << "[ Simul. Tasks Average Time: " << averageTime << " sec\t\t Number of Threads: " << client->getThreads() << "\t\t Number of Trials: " << trials; 
        cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " tasks/sec ]\n" << endl;
        cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    }
 
    if (csvOption && (num == 0 || num == 100))
    {
        *csv << "\n" << "\n";
    }
 
    return averageTime;
}
 
 
 
double BenchmarkManager::getTransactionsOutput(int threads, int start, bool csvOption, int num)
{
    if (num <= 10 && csvOption)
    {
        *csv << threads << " Simul. Transactions,Threads,Runs,Time Elapsed (sec),Average Time per Tasks\n";
    }
 
    client->setThreads(threads);
 
    vector<double> times;
 
    int max = (num == 0) ? trials : 1;
 
    for (int i = 1; i <= max; i++)
    {
        double time = client->performTransactions(start);
 
        if (num == 0)
        {
            if (showOutputs)
            {
                cout << threads << " Simultaneous Transactions #" << i << ":\t\t\t";
                cout << "time: " << time << " sec\n" << endl;
            }
        }
 
        i = (num == 0) ? i : num;

        if (csvOption)
        {
            *csv << i << "," << client->getThreads() << "," << client->getRuns() << "," << time << "\n";
        }

        times.push_back(time);
    }
 
    double sumTime = 0;
 
    for (double t : times)
    {
        sumTime += t;
    }
    
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getRuns()) / sumTime; 
 
    if (num == 0)
    {
        cout << "\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
        cout << "[ Simul. Transactions Average Time: " << averageTime << " sec\t\t Number of Threads: " << client->getThreads() << "\t\t Number of Trials: " << trials; 
        cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " transactions/sec ]\n" << endl;
        cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    }
 
    if (csvOption && (num == 0 || num == 100))
    {
        *csv << "\n" << "\n";
    }
 
    return averageTime;
}
 

