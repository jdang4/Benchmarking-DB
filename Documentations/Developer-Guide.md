# Developer's Guide to Application

The README file was to describe how to set up and run this application, and this file is meant to describe the structure of the application.


# Overall Structure and Flow of the Application

```console
                       ----------------                   ----------------------                   -----------------
                       |              |                   |                    |                   |               |
                       |              |                   |                    | ----------------> |               |
                       |              |                   |                    |                   |               |
user ----------------> |  benchmark   | ----------------> |  BenchmarkManager  |                   |   DBClient    |
                       |              |                   |                    | <---------------- |               |
                       |              |                   |                    |                   |               |
                       |              |                   |                    |                   |               |
                       ----------------                   ----------------------                   -----------------

```

This project is split across 3 main files: **benchmark**, **BenchmarkManager**, **DBClient**. 

The role of the benchmark.cpp file is to collect all the options defined by the user and then communicate those options to the BenchmarkManager. Some of the options that can be defined by the user are the number of entries to operate on for each benchmark test, the number of threads to use, which DB client to use, etc. The main responsibility of the file is to communicate with the BenchmarkManager and tell it how it should communicate with the DB client.

The BenchmarkManager essentially acts as the manager for the DBClient. This class has 2 main functionalities; it acts as the main communicator between the main client (the benchmark.cpp file) and the DBClient, and it produces the print outputs that are shown when the application is running. An example of how its functionality as the main communicator operates, if the user wanted to add randomization to the key, the benchmark.cpp file would notify the BenchmarkManager to set that option to "true" and the manager would relay that message to the used database client telling the client to randomize the key as it performs the benchmark test. Keep in mind that this file does not communicate directly with the implemented DB client, only with the abstract client (DBCient). In doing so, it allows for flexibility in adding additional db clients.

The DBClient acts an abstraction for the implemented DB clients (RedisClient and PostgresClient) and is what is being used to communicate with the BenchmarkManager. It includes all the methods that the BenchmarkManager would need to know of. While its methods are predefined, the actual implementation of it are overrided by the subclasses. Currently, the 2 subclasses of DBClient are RedisClient and PostgresClient. These classes composes of the actual implementation of the database client (where it makes the direct queries to the database). Each of the methods that are used to perform the benchmark on the operation defines a lambda function and passes that function as a parameter to a method that creates and runs the threads aka "run_threads". 


## General Overview of How the Database Client's Methods Work 

Each method that does a benchmark on an operation defines a lambda function that performs follows the general flow:
    
    1) creates a connection to the database, 
    2) defines the lambda function where it performs the specific operation/transaction,
    3) disconnects from the database. 

After defining the lambda function, call the "run_threads" method and pass the lambda function as a parameter for that method. Return the double value, which defines the total time it took to complete performing the benchmark. For a clearer example of how these methods are defined, please take a look at the source code for RedisClient.cpp and PostgresClient.cpp.

The "run_threads" method is responsible for creating and making use of the threads during the benchmark. Its main responsibility is to define how many entries each of the available threads would manager and it would add the provided lambda function to the thread functionality. As the method is waiting for all the threads to finish, it is timing how long it takes to do so and it would return the elapsed time as a double value. The method that calls the "run_threads" method should return the double value that the "run_threads" method returns.


# Guides to the Used C++ DB Clients

Please click on the following links below to see the used C++ db clients:

* [Redis C++ Client](https://github.com/sewenew/redis-plus-plus)

* [PostgreSQL C++ Client](https://github.com/jtv/libpqxx)


# Adding Additional Database Clients

I tried to make this step as simple as possible by using the concept of inheritance. By doing so, the BenchmarkManager would not need to know too much of the implementation of the database client other than its existence. The most critical steps when adding an additional database client are to define the added db client to inherit from the base class (DBClient) and to override the implementation of the methods defined in the base class. There are some methods within the DBClient that should not be overrided that are used as helper methods. When inserting/updating an entry, to get a 6K byte value, you can call the "getEntryVal" method, which would return a 6K byte value. The other method is "calculateTime" which takes 2 chrono::time_point values and calculates the elapsed time based on the provided timestamps. Please refer to the documentation of the 2 methods in the DBClient.cpp file for more details.

Also when implementing the new database client, it is also important to define a "run_threads" method. Because it utilizes the provided lambda function in its parameters I could not define this method within the DBClient and so this is essentially a method whose implementation is shared among the subclasses of the DBClient. For implementing this method you can simply copy and paste the implmentaion of from a different subclass. 

## Adjusting the Dockerfile

Because the project is ran in a Docker environment, it is necessary (if needed) to add the installation of the C++ database client to the Dockerfile. For organization purposes,in the Dockerfile please make the install of the database client inside the "include" directory.


## Areas to Make the Small Adjustments in the Code

Since this application is recording the outputted numbers into a CSV file and also handling different database clients, I named each of the CSV files by the different database names. For example, if I was benchmarking Redis, the application would record the data into a CSV file with Redis in the name of the file. Due to this implementation, when adding a new database client, it is necessary to make an edit within the application that allows the program to record the data into a CSV file. In the "benchmark.cpp" file, there is a string variable named "file", and that is where the edit should be made. On a similar note, it is also important to include the option to select the new database in the user prompt as well. These are very minor edits that are still important to make when adding a new database client.

