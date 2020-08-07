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
 

/**
 * Selects the db client to use for the benchmark
 * 
 * @param db - the db to benchmark
 * @param host - the host of the primary container to connect to
 */
void BenchmarkManager::selectDB(int db, string host)
{
    dbClient = db;

    switch (db)
    {
    case 1:
        client = new RedisClient(host);
        dbName = "REDIS";
        break;

    case 2:
        client = new PostgresClient(host);
        dbName = "POSTGRESQL";
        break;

    default:
        cout << "INVALID DB CHOICE!" << endl;
        exit(1);
    }
}

/**
 * Sets the number of threads and the number of entries to use during the benchmark test
 * 
 * @param threads - denotes the number of threads to use
 * @param runs - denotes the number of entries to handle in each test
 */
void BenchmarkManager::setThreads_and_Entries(int threads, int entries)
{
    client->setThreads(threads);
    client->setEntries(entries);
}


/**
 * Makes the connection to the database server
 * 
 */
void BenchmarkManager::connect()
{
    client->connect();
    cout << "------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    
}


void BenchmarkManager::disconnect()
{
    cout << dbName << " " << "DISCONNECTED" << endl;
}


/**
 * This method calls the db client to initialize the DB
 * 
 */
void BenchmarkManager::initializeDB()
{
    double time = client->initializeDB();

    if (time != -1.0)
    {
        cout << "time: " << time << " sec" << endl;
    }

    cout << "\n------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
 
}


/**
 * This method is called to open the CSV to record the produced data from the benchmark
 * 
 * @param n - denotes which DB CSV file to record the data to
 */
void BenchmarkManager::openCSV(int n)
{
    string file;

    switch (dbClient)
    {
    case 1:
        file = "stats/" + to_string(n) + "/redis-stats.csv";
        break;

    case 2:
        file = "stats/" + to_string(n) + "/postgres-stats.csv";
        break;

    default:
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

/**
 * Closes the CSV file
 * 
 */
void BenchmarkManager::closeCSV()
{
    csv->close();
}

/**
 * This method is called to perform the randomizing benchmark test. Both the operation and the 
 * keys (if able to) are randomized
 * 
 */
void BenchmarkManager::performRandomization()
{
    srand(time(0));

    int option = (rand() % 6) + 1;

    int starting_key = (rand() % (3000000 + 1 - 2000000)) + 2000000;

    int thread_counts[3] = {1, 5, 10};

    int index = (rand() % (2 + 1));

    int thread = thread_counts[index];

    string title;

    trials = 1;

    switch (option)
    {
    case 1:
        title = "READING";
        cout << title << " BENCHMARK: \n\n" << endl;
        cout << thread << " THREAD: \n\n" << endl;
        getReadOutput(thread, false, true);
        cout << endl;
        break;

    case 2:
        title = "INSERTION";
        cout << title << " BENCHMARK: \n\n" << endl;
        cout << thread << " THREAD: \n\n" << endl;
        getInsertOutput(thread, starting_key, false);
        cout << endl;
        getDeleteOutput(thread, starting_key, false, false);
        cout << endl;

        break;

    case 3:
        title = "UPDATING";
        cout << title << " BENCHMARK: \n\n" << endl;
        cout << thread << " THREAD: \n\n" << endl;
        getUpdateOutput(thread, 1, false, true);
        cout << endl;

        break;

    case 4:
        title = "DELETION";
        cout << title << " BENCHMARK: \n\n" << endl;
        cout << thread << " THREAD: \n\n" << endl;
        getDeleteOutput(thread, 1, false, true);
        cout << endl;

        break;

    case 5:
        title = to_string(thread) + " SIMULTANEOUS TASKS";
        cout << title << " BENCHMARK: \n\n" << endl;
        cout << thread << " THREAD: \n\n" << endl;
        getSimultaneousTasksOutput(thread, false, true);
        cout << endl;

        break;

    case 6:
        title = to_string(thread) + " SIMULTANEOUS TRANSACTIONS";
        cout << title << " BENCHMARK: \n\n" << endl;
        cout << thread << " THREAD: \n\n" << endl;
        getTransactionsOutput(thread, starting_key, false);
        cout << endl;

        break;

    default:
        cout << "ERROR" << endl;
        exit(-1);
    }
}


/**
 * Performs the benchmark on the Read operation
 * 
 * @param threads - the number of threads to use
 * @param csvOption - whether to write to the CSV file or not
 * @param random - option to randomize the key
 * 
 * @return the average time of doing a Read operation 
 */
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
             *csv << i << "," << client->getThreads() << "," << client->getEntries() << "," << time << "\n";
        }
       
 
        times.push_back(time);
 
    }
 
    double sumTime = 0;
 
    for (double t : times)
    {
        sumTime += t;
    }
 
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getEntries()) / sumTime;
 
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
 
 
/**
 * Performs the benchmark on the Insert operation
 * 
 * @param threads - the number of threads to use
 * @param csvOption - whether to write to the CSV file or not
 * 
 * @return the average time of doing a Insert operation 
 */
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

        // done to avoid existing entry error when doing inserts
        key += 1000000;
 
        if (showOutputs)
        {
            cout << "INSERTION #" << i << ":\t\t\t\t\t";
            cout << "time: " << time << " sec\n" << endl;
        }

        if (csvOption)
        {
            *csv << i << "," << client->getThreads() << "," << client->getEntries() << "," << time << "\n";
        }
        
 
        times.push_back(time);
 
    }
 
    double sumTime = 0; 
 
    for (double t : times)
    {
        sumTime += t;
    }
 
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getEntries()) / sumTime;
 
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
 
 
/**
 * Performs the benchmark on the Update operation
 * 
 * @param threads - the number of threads to use
 * @param start = starting key value
 * @param csvOption - whether to write to the CSV file or not
 * @param random - option to randomize the key
 * 
 * @return the average time of doing a Update operation 
 */
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
            *csv << i << "," << client->getThreads() << "," << client->getEntries() << "," << time << "\n";
        }
        
 
        times.push_back(time);
 
    }
 
    double sumTime = 0;
 
    for (double t : times)
    {
        sumTime += t;
    }
 
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getEntries()) / sumTime;
 
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
 
 
/**
 * Performs the benchmark on the Delete operation
 * 
 * @param threads - the number of threads to use
 * @param start = starting key value
 * @param csvOption - whether to write to the CSV file or not
 * @param random - option to randomize the key
 * 
 * @return the average time of doing a Delete operation 
 */
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
            *csv << i << "," << client->getThreads() << "," << client->getEntries() << "," << time << "\n";
        }
        
        
        times.push_back(time);
    }
 
    double sumTime = 0;
 
    for (double t : times)
    {
        sumTime += t;
    }
 
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getEntries()) / sumTime;
 
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
 

/**
 * Performs the benchmark on the Tasks operation
 * 
 * @param threads - the number of threads to use
 * @param csvOption - whether to write to the CSV file or not
 * @param random - option to randomize the key
 * 
 * @return the average time of doing a Tasks operation 
 */
double BenchmarkManager::getSimultaneousTasksOutput(int threads, bool csvOption, bool random)
{
    if (csvOption)
    {
        *csv << threads << " Simul. Tasks Num,Threads,Runs,Time Elapsed (sec),Average Time per Tasks\n";
    }
 
    vector<double> times;
 
    client->setThreads(threads);
 
    for (int i = 1; i <= trials; i++)
    {
        double time = client->simultaneousTasks(random);

        if (showOutputs)
        {
            cout << threads << " Simultaneous Tasks #" << i << ":\t\t\t";
            cout << "time: " << time << " sec\n"
                 << endl;
        }

        if (csvOption)
        {
            *csv << i << "," << client->getThreads() << "," << client->getEntries() << "," << time << "\n";
        }
 
        times.push_back(time);
    }
 
    double sumTime = 0;
 
    for (double t : times)
    {
        sumTime += t;
    }
 
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getEntries()) / sumTime; 

    cout << "\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Simul. Tasks Average Time: " << averageTime << " sec\t\t Number of Threads: " << client->getThreads() << "\t\t Number of Trials: " << trials; 
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " tasks/sec ]\n" << endl;
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl; 
 
    if (csvOption)
    {
        *csv << "\n" << "\n";
    }
 
    return averageTime;
}
 
 
/**
 * Performs the benchmark on the Transactions operation
 * 
 * @param threads - the number of threads to use
 * @param csvOption - whether to write to the CSV file or not
 * @param random - option to randomize the key
 * 
 * @return the average time of doing a Transactions operation 
 */
double BenchmarkManager::getTransactionsOutput(int threads, int start, bool csvOption)
{
    if (csvOption)
    {
        *csv << threads << " Simul. Transactions,Threads,Runs,Time Elapsed (sec),Average Time per Tasks\n";
    }
 
    client->setThreads(threads);
 
    vector<double> times;
 
    for (int i = 1; i <= trials; i++)
    {
        double time = client->performTransactions(start);

        if (showOutputs)
        {
            cout << threads << " Simultaneous Transactions #" << i << ":\t\t\t";
            cout << "time: " << time << " sec\n"
                 << endl;
        }

        if (csvOption)
        {
            *csv << i << "," << client->getThreads() << "," << client->getEntries() << "," << time << "\n";
        }

        times.push_back(time);
    }
 
    double sumTime = 0;
 
    for (double t : times)
    {
        sumTime += t;
    }
    
    double averageTime = sumTime / double(times.size());
 
    double avg_op_per_sec = double(times.size() * client->getEntries()) / sumTime; 
 
    cout << "\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;
    cout << "[ Simul. Transactions Average Time: " << averageTime << " sec\t\t Number of Threads: " << client->getThreads() << "\t\t Number of Trials: " << trials; 
    cout << "\t\t Total Time: " << sumTime << " sec\t\t" << " Estimated " << avg_op_per_sec << " transactions/sec ]\n" << endl;
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    if (csvOption)
    {
        *csv << "\n" << "\n";
    }
 
    return averageTime;
}
 

