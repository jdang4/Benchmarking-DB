# Benchmarking DB Application

This project is to offer a benchmark application that is able to help in the selection of a Cloud Database to use in the Control/User Plane Separation (CUPS) project.
In this project, it is capable of handling multiple of different database clients to benchmark against and it will record to recorded data into a generated CSV file for 
further analysis on the information.

# Table of Contents  
1. [Before Running](#before-running)  
2. [Running the Application](#running-the-application)  
    1. [Running the Application from Different Host](#running-the-application-diff-host)
3. [Customize the Config file for the Database Servers](#customize-configs)
4. [Understanding the User Options in Benchmark App](#options-in-app)
6. [Possible Errors that May Occur When Running App](#errors)
7. [Navigativing Through Stats Directory](#stats)
5. [Contact](#contact)



# Before Running <a name="before-running"></a>

This application is designed to run in an individual docker container and connects with the running database servers that are all in their own docker containers as well. So before running this application, it is required that the containers that hold the database server are up and running.


## How to Deploy a Docker Container with a Database Server

### Redis

My reccomendation is to get the Bitnami Redis Docker Image is to pull the prebuild image from the Docker Hub Registry. I have found it straight-forward to get a replication going when using this image. Also, the example commands I have provided are used when using this image.

```console
$ docker pull bitnami/redis:latest
```

After pulling the prebuilt image you can go ahead and get the Redis container running in the Docker Engine. The following commands is what I use that includes my own Redis config file, persistence, and replication. 

> NOTE: Please note that the filepaths included in my commands must be changed to fit with your local machine. There is no need to edit the path that follows after the ":"
in the file path, only the path before the colon. In this case, change the following path to the project directory "~/docker/Benchmark/" to fit your local machine.


**Redis Primary Container:**

```console
$ docker run --name redis-master -v ~/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data -e REDIS_REPLICATION_MODE=master -e ALLOW_EMPTY_PASSWORD=yes --rm -d bitnami/redis:latest
```

**Redis Replica Container:**

```console
$ docker run --name redis-replica -v ~/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data --link redis-master:master -e REDIS_REPLICATION_MODE=slave -e REDIS_MASTER_HOST=master -e ALLOW_EMPTY_PASSWORD=yes --rm -d bitnami/redis:latest
```

For more information on Bitnami Redis, please refer to the [bitnami-docker-redis](https://github.com/bitnami/bitnami-docker-redis) repository.

### 

### PostgreSQL

My reccomendation is to get the Bitnami PostgreSQL Docker Image is to pull the prebuilt image from the Docker Hub Registry. I have found it straight-forward to get a replication going when using this image. Also, the example commands I have provided are used when using this image.

```console
$ docker pull bitnami/postgresql:latest
```

After pulling the prebuilt image you can go ahead and get the PostgreSQL container running in the Docker Engine. The following commands is what I use that includes my own PostgreSQL config file, persistence, and replication. Please note that the filepaths included in my commands must be changed to fit with your system.

> NOTE: When using your own username/password, please make the edit in the PostgresClient.cpp file in order to allow the application to make a connection

**PostgreSQL Primary Container:**

```console
$ docker run -itd --rm --name postgres-master -v ~/postgres-data/master:/bitnami/postgresql -v ~/docker/Benchmark/config/conf.d/:/bitnami/postgresql/conf/conf.d -e POSTGRESQL_USERNAME=postgres -e POSTGRESQL_PASSWORD=Juni#20 -e POSTGRESQL_DATABASE="SDB" -e POSTGRESQL_REPLICATION_MODE=master -e POSTGRESQL_REPLICATION_USER=primary -e POSTGRESQL_REPLICATION_PASSWORD=replication bitnami/postgresql:latest
```

**PostgreSQL Replica Container:**

```console
$ docker run -itd --rm --name postgres-replica --link postgres-master:master -v ~/postgres-data/replica:/bitnami/postgresql -v ~/docker/Benchmark/config/conf.d/:/bitnami/postgresql/conf/conf.d -e POSTGRESQL_USERNAME=postgres -e POSTGRESQL_PASSWORD=Juni#20 -e POSTGRESQL_DATABASE="SDB" -e POSTGRESQL_REPLICATION_MODE=slave -e POSTGRESQL_MASTER_HOST=master -e POSTGRESQL_REPLICATION_USER=primary -e POSTGRESQL_REPLICATION_PASSWORD=replication bitnami/postgresql:latest
```

For more information on Bitnami PostgreSQL, please refer to the [bitnami-docker-postgresql](https://github.com/bitnami/bitnami-docker-postgresql) repository.


# Running the Application <a name="running-the-application"></a>

After getting the database servers up and running in a Docker container, you can now proceed to run the application. Before running, you would first need to build the application image.

**Building the application image:**

```console
$ docker build --tag <insert image name> .
```

**Running the application:**

```console
$ docker run -it --rm --name benchmark-client --link postgres-master:bitnami/postgresql --link redis-master:bitnami/redis -v ~/docker/Benchmark/stats:/benchmark/stats <insert image name>
```


## Running the Application from a Different Host <a name="running-the-application-diff-host"></a>

### 1) Set up the Docker Swarm

1) On host1, initialize the Docker Swarm:

```console
$ docker swarm init

Swarm initialized: current node (2jav4ee4hve19vgixivxeo31i) is now a manager.

To add a worker to this swarm, run the following command:

    docker swarm join --token SWMTKN-1-47b49oudwpt8kwrynatihzhv51plnkt2446u2b2szzr8seonsu-31gdd1zla3tfy867gmkx1jp2a 10.9.161.11:2377

To add a manager to this swarm, run 'docker swarm join-token manager' and follow the instructions.
```

2) On host2, join the swarm as instructed above:

```console
$ docker swarm join --token <insert token> <insert ip address>:2377
```

### 2) Create an Attachable Overlay Network

On host1, create an attachable overlay network:

```console
$ docker network create --driver=overlay --attachable benchmark-network
```

After creating the network, still on host1, run the containers containing the database servers while including the "-it" and "--network benchmark-network" (benchmark-network is changeable) flags somwhere within the lines of the run command to connect to your created network. Please refer to the examples for Redis and PostgreSQL below as a guide. Again, please adjust the file path according and names.

**Redis Primary**

```console
$ docker run --name redis-master -v ~/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data -e REDIS_REPLICATION_MODE=master -e ALLOW_EMPTY_PASSWORD=yes --rm -itd --network benchmark-network bitnami/redis:latest
```

**Redis Replica**

```console
$ docker run --name redis-replica -v ~/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data --link redis-master:master -e REDIS_REPLICATION_MODE=slave -e REDIS_MASTER_HOST=master -e ALLOW_EMPTY_PASSWORD=yes --rm -itd --network benchmark-network bitnami/redis:latest
```

**PostgreSQL Primary**

```console
$ docker run -itd --rm --name postgres-master -v ~/postgres-data/master:/bitnami/postgresql -v ~/docker/Benchmark/config/conf.d/:/bitnami/postgresql/conf/conf.d -e POSTGRESQL_USERNAME=postgres -e POSTGRESQL_PASSWORD=Juni#20 -e POSTGRESQL_DATABASE="SDB" -e POSTGRESQL_REPLICATION_MODE=master -e POSTGRESQL_REPLICATION_USER=primary -e POSTGRESQL_REPLICATION_PASSWORD=replication --network benchmark-network bitnami/postgresql:latest
```

**PostgreSQL Replica**

```console
$ docker run -itd --rm --name postgres-replica --link postgres-master:master -v ~/postgres-data/replica:/bitnami/postgresql -v ~/docker/Benchmark/config/conf.d/:/bitnami/postgresql/conf/conf.d -e POSTGRESQL_USERNAME=postgres -e POSTGRESQL_PASSWORD=Juni#20 -e POSTGRESQL_DATABASE="SDB" -e POSTGRESQL_REPLICATION_MODE=slave -e POSTGRESQL_MASTER_HOST=master -e POSTGRESQL_REPLICATION_USER=primary -e POSTGRESQL_REPLICATION_PASSWORD=replication --network benchmark-network bitnami/postgresql:latest
```

### 3) Get the network to be created on host2

From my experience, in order to get the network that was created on host1, I had to create a running container that connects to that network on host2. Once I am able to verify that the network that was created on host1 is now created on host2, I can then run the application on a different host

```console
$ docker run --name running-redis -e ALLOW_EMPTY_PASSWORD=yes --rm -itd --network benchmark-network bitnami/redis:latest
```

**Running the application from different host**

> NOTE: the stats directory is provided in this repository

```console
$ docker run -it --rm --network benchmark-network -v ~/docker/Benchmarking-DB/stats:/benchmark/stats <insert image name of app>
```

### 

For a more in-depth reference please visit the [Docker Swarm Walkthrough](https://docs.docker.com/network/network-tutorial-overlay/).


# Customize the Config file for the Database Servers <a name="customize-configs"></a>

## Redis

In the repository there exists a config folder. In order to edit the config file for Redis, please make the edit to the following file: config/redis.conf

## PostgreSQL

In the repository there exists a config folder. In order to edit the config file for PostgreSQL, please make the edit to the following file: config/conf.d/extended.conf


# Understanding the User Options in Benchmark App <a name="options-in-app"></a>

When running the application, the user would be given options to choose from:

* Selecting a Database
    
* Whether to show each individual print output and not just show the summary of the benchmark for the particular operation
    
* How long you want the program to run. It uses a while loop to repeatably run the whole benchmark test and stores the collected data into a CSV file with "running" in the name. To run the benchmark tests once, use a very small double value. Also, keep in mind that when using a small number of entries (1 entry) for each operation test, you may want to do a small value like 0.1 to get the program to run the tests in only one round. Smaller number of entries being used for each test tend to allow the program to finish quickly
    
* Select how you want to test the operations. The first performs a test on each of the operations used without randomizing the keys, and the second one does the same as the first but it randomizes the key. The third option would randomly select an operation to benchmark and does a test on it while randomizng the keys

* Select the Record Size that you want to benchmark against
    
* Select the number of entries you want to perform on each operation


# Possible Errors that May Occur When Running App <a name="errors"></a>

I have found that as the program runs for a while eventually it would cause out of memory errors, causing the program to fail. Although I am sure there are better solutions than mine, but for a quick solution, I cleared the folders where the persistent files of the database would reside in and reboot the database containers (stopping then starting it up again). Again, please note that this is not the most effective solution and is just one simple way to get the application working again.


# Navigativing Through Stats Directory <a name="stats"></a>

When going into the stats directory, you'll see different folders, those folders indicate the record sizes. Choose the desired directory and in that directory you'll see additional directories. Those directories correspond the number of entries that were being used in the test, and selecting the desired one you'll be able to see the generated csv files.


# Contact

If you have any additional questions about this project, feel free to contact me at jdang@wpi.edu.
