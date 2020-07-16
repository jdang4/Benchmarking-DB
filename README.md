# Benchmarking DB Application

#

# Before Running

This application is designed to run in an individual docker container and connects with the running database servers that are all in their own docker containers as well. So before running this application, it is required that the containers that hold the database server are up and running.


## How to Deploy a Docker Container with a Database Server

### Redis

My reccomendation is to get the Bitnami Redis Docker Image is to pull the prebuild image from the Docker Hub Registry.

```console
$ docker pull bitnami/redis:latest
```

After pulling the prebuilt image you can go ahead and get the Redis container running in the Docker Engine. The following commands is what I use that includes my own Redis config file, persistence, and replication. Please note that the filepaths included in my commands must be changed to fit with your system.


**Redis Primary Container:**

```console
$ docker run --name redis-master -v ~/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data -e REDIS_REPLICATION_MODE=master -e ALLOW_EMPTY_PASSWORD=yes --rm -d bitnami/redis:latest
```

**Redis Replica Container:**

```console
$ docker run --name redis-replica -v /home/jonathand/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data --link redis-master:master -e REDIS_REPLICATION_MODE=slave -e REDIS_MASTER_HOST=master -e ALLOW_EMPTY_PASSWORD=yes --rm -d bitnami/redis:latest
```

For more information on Bitnami Redis, please refer to the [bitnami-docker-redis](https://github.com/bitnami/bitnami-docker-redis) repository.

#

### PostgreSQL

My reccomendation is to get the Bitnami PostgreSQL Docker Image is to pull the prebuilt image from the Docker Hub Registry.

```console
$ docker pull bitnami/postgresql:latest
```

After pulling the prebuilt image you can go ahead and get the PostgreSQL container running in the Docker Engine. The following commands is what I use that includes my own Redis config file, persistence, and replication. Please note that the filepaths included in my commands must be changed to fit with your system.

> NOTE: To edit the PostgreSQL config file, follow the path in this repository config/conf.d/ and make your adjustments in the extended.conf file

**PostgreSQL Primary Container:**

```console
$ docker run -itd --rm --name postgres-master -v ~/postgres-data/master:/bitnami/postgresql -v ~/docker/Benchmark/config/conf.d/:/bitnami/postgresql/conf/conf.d -e POSTGRESQL_USERNAME=postgres -e POSTGRESQL_PASSWORD=<insert password> -e POSTGRESQL_DATABASE="SDB" -e POSTGRESQL_REPLICATION_MODE=master -e POSTGRESQL_REPLICATION_USER=primary -e POSTGRESQL_REPLICATION_PASSWORD=replication bitnami/postgresql:latest
```

**PostgreSQL Replica Container:**

```console
$ docker run -itd --rm --name postgres-replica --link postgres-master:master -v ~/postgres-data/replica:/bitnami/postgresql -v ~/docker/Benchmark/config/conf.d/:/bitnami/postgresql/conf/conf.d -e POSTGRESQL_USERNAME=postgres -e POSTGRESQL_PASSWORD=<insert password> -e POSTGRESQL_DATABASE="SDB" -e POSTGRESQL_REPLICATION_MODE=slave -e POSTGRESQL_MASTER_HOST=master -e POSTGRESQL_REPLICATION_USER=primary -e POSTGRESQL_REPLICATION_PASSWORD=replication bitnami/postgresql:latest
```

For more information on Bitnami PostgreSQL, please refer to the [bitnami-docker-postgresql](https://github.com/bitnami/bitnami-docker-postgresql) repository.


# Running the Application

After getting the database servers up and running in a Docker container, you can now proceed to run the application. Before running, you would first need to build the application image.

**Building the application image:**

```console
$ docker build --tag <insert image name> .
```

**Running the application:**

```console
$ docker run -it --rm --name benchmark-client --link postgres-master:bitnami/postgresql --link redis-master:bitnami/redis -v ~/docker/Benchmark/stats:/benchmark/stats <insert image name>
```


# Running the Application from a Different Host

## 1) Set up the Docker Swarm

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
$ docker swarm joing --token <insert token> <insert ip address>:2377
```

## 2) Create an Attachable Overlay Network

On host1, create an attachable overlay network:

```console
$ docker network create --driver=overlay --attachable benchmark-network
```

After creating the network, still on host1, run the containers containing the database servers while including the "-it" and "--network benchmark-network" flags somwhere within the lines of the run command to connect to your created network. Please refer to the example below as a guide.

```console
$ docker run --name redis-master -v ~/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data -e REDIS_REPLICATION_MODE=master -e ALLOW_EMPTY_PASSWORD=yes --rm -itd --network benchmark-network bitnami/redis:latest
```

## 3) Get the network to be created on host2

From my experience, in order to get the network that was created on host1, I had to create a running container that connects to that network. Once I am able to verify that the network that was created on host1 is now created on host2, I can then run the application on a different host

```console
$ docker run --name running-redis -e ALLOW_EMPTY_PASSWORD=yes --rm -itd --network benchmark-network bitnami/redis:latest
```

**Running the application from different host**

> NOTE: you would need to edit the filepath to the stats directory

```console
$ docker run -it --rm --name jonathan-client --network benchmark-network -v ~/docker/Benchmarking-DB/stats:/benchmark/stats jonathan-cpp
```

#

For a more in-depth reference please visit the [Docker Swarm Walkthrough](https://docs.docker.com/network/network-tutorial-overlay/).