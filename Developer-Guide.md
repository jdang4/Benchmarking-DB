# Developer's Guide to Application

The README file was to describe how to set up and run this application, and this file is meant to describe the structure of the application.


# Overall Structure of the Application

This project is split across 5 main files: benchmark, BenchmarkManager, DBClient, RedisClient, and PostgresClient. 

In the benchmark.cpp file, it defining all the paramaters that will be used throughout the project, the number of entries to operate on for each benchmark test, the number of threads to use, which DB client to use, etc. The main responsibility of the file is to communicate with the BenchmarkManager and tell it how it should communicate with the DB client.

The BenchmarkManager essentially acts as the manager for the DBClient. Its main responsibility is to recieve information from the benchmark.cpp file and use that information
to communicate with the DBClient. Only the manager makes the direct communication to the DB client. It's main responsibility is to tell the client to perform the test on the operation and it will produce the resulting information of the test. This file does not communicate directly with the implemented DB client, only with the abstract client (DBCient). In doing so, it allows for flexibility in adding additional db clients.

The DBClient acts an abstraction for the implemented DB clients (RedisClient and PostgresClient) and is what is being used to communicate with the client, aka the BenchmarkManager. It includes all the methods that the BenchmarkManager would need to know of and that are shared among the implemented clients. While its methods are predefined, after the BenchmarkManager selects which database it wants to use, the method that was defined by that database's client will be ran instead. 

The RedisClient and PostgresClient files composes of the actual implementation, where it makes the direct queries to the database. Each of the methods that are used to perform the benchmark on the operation defines a lambda function and passes that function as a parameter to a method that creates and runs the threads. 