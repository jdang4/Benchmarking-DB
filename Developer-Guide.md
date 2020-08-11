# Developer's Guide to Application

The README file was to describe how to set up and run this application, and this file is meant to describe the structure of the application.


# Overall Structure and Flow of the Application

                     ----------------                   ----------------------                       -----------------
                     |              |                   |                    |                       |               |
                     |              |                   |                    | --------------------> |               |
                     |              |                   |                    |                       |               |
user --------------> |  benchmark   | ----------------> |  BenchmarkManager  |                       |   DBClient    |
                     |              |                   |                    | <-------------------- |               |
                     |              |                   |                    |                       |               |
                     |              |                   |                    |                       |               |
                     ----------------                   ----------------------                       -----------------


This project is split across 3 main files: benchmark, BenchmarkManager, DBClient. 

In the benchmark.cpp file, it sets all the paramaters that will be used throughout the project that are defined by the user. Some of these parameters are the number of entries to operate on for each benchmark test, the number of threads to use, which DB client to use, etc. The main responsibility of the file is to communicate with the BenchmarkManager and tell it how it should communicate with the DB client.

The BenchmarkManager essentially acts as the manager for the DBClient. This class has 2 main functionalities; it acts as the main communicator between the main client (the benchmark.cpp file) and the database client, and it produces the print outputs that are shown when the application is running. An example of how its functionality as the main communicator operates, if the user wanted to add randomization to the key, the benchmark.cpp file would notify the BenchmarkManager to set that option to "true" and the manager would relay that message to the used database client telling the client to randomize the key as it performs the benchmark test. Keep in mind that this file does not communicate directly with the implemented DB client, only with the abstract client (DBCient). In doing so, it allows for flexibility in adding additional db clients.

The DBClient acts an abstraction for the implemented DB clients (RedisClient and PostgresClient) and is what is being used to communicate with the BenchmarkManager. It includes all the methods that the BenchmarkManager would need to know of. While its methods are predefined, the actual implementation of it are overrided by the subclasses. Currently, the 2 subclasses of DBClient are RedisClient and PostgresClient. These classes composes of the actual implementation of the database client (where it makes the direct queries to the database). Each of the methods that are used to perform the benchmark on the operation defines a lambda function and passes that function as a parameter to a method that creates and runs the threads aka "run_threads". 


## General Overview of How the Database Client's Methods Work 

Each method that does a benchmark on an operation defines a lambda function that performs follows the general flow:
    
    1) creates a connection to the database, 
    2) defines the lambda function where it performs the specific operation/transaction,
    3) disconnects from the database. 

After defining the lambda function, call the "run_threads" method and pass the lambda function as a parameter for that method. Return the double value, which defines the total time it took to complete performing the benchmark. 

The "run_threads" method is responsible for creating and making use of the threads during the benchmark. Its main responsibility is to define how many entries each of the available threads would manager and it would add the provided lambda function to the thread functionality. As the method is waiting for all the threads to finish, it is timing how long it takes to do so and it would return the elapsed time as a double value. The method that calls the "run_threads" method should return the double value that the "run_threads" method returns.