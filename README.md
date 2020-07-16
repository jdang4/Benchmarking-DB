# Benchmarking DB Application


# Before Running

This application is designed to run in an individual docker container and connects with the running database servers that are all in their own docker containers as well. So before running this application, it is required that the containers that hold the database server are up and running.


## How to Deploy a Docker Container with a Database Server

### Redis

My reccomendation is to get the Bitnami Redis Docker Image is to pull the prebuild image from the Docker Hub Registry.

```console
$ docker pull bitnami/redis:latest
```

After pulling the prebuilt image you can go ahead and get the Redis container running in the Docker Engine. The following commands is what I use that includes my own Redis config file, persistence, and replication. Please note that the filepaths included in my commands must be changed to fit with your system.


Redis Primary Container:

```console
$ docker run --name redis-master -v ~/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data -e REDIS_REPLICATION_MODE=master -e ALLOW_EMPTY_PASSWORD=yes --rm -d bitnami/redis:latest
```

Redis Replica Container:

```console
$ docker run --name redis-replica -v /home/jonathand/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data --link redis-master:master -e REDIS_REPLICATION_MODE=slave -e REDIS_MASTER_HOST=master -e ALLOW_EMPTY_PASSWORD=yes --rm -d bitnami/redis:latest
```

For more information on Bitnami Redis, please refer to the [bitnami-docker-redis](https://github.com/bitnami/bitnami-docker-redis) repository.

### PostgreSQL

My reccomendation is to get the Bitnami PostgreSQL Docker Image is to pull the prebuilt image from the Docker Hub Registry.

```console
$ docker pull bitnami/postgresql:latest
```

After pulling the prebuilt image you can go ahead and get the PostgreSQL container running in the Docker Engine. The following commands is what I use that includes my own Redis config file, persistence, and replication. Please note that the filepaths included in my commands must be changed to fit with your system.

> NOTE: To edit the PostgreSQL config file, follow the path in this repository config/conf.d/ and make your adjustments in the extended.conf file

PostgreSQL Primary Container:

```console
$ docker run -itd --rm --name postgres-master -v ~/postgres-data/master:/bitnami/postgresql -v ~/docker/Benchmark/config/conf.d/:/bitnami/postgresql/conf/conf.d -e POSTGRESQL_USERNAME=postgres -e POSTGRESQL_PASSWORD=<insert password> -e POSTGRESQL_DATABASE="SDB" -e POSTGRESQL_REPLICATION_MODE=master -e POSTGRESQL_REPLICATION_USER=primary -e POSTGRESQL_REPLICATION_PASSWORD=replication bitnami/postgresql:latest
```

PostgreSQL Replica Container:

```console
$ docker run -itd --rm --name postgres-replica --link postgres-master:master -v ~/postgres-data/replica:/bitnami/postgresql -v ~/docker/Benchmark/config/conf.d/:/bitnami/postgresql/conf/conf.d -e POSTGRESQL_USERNAME=postgres -e POSTGRESQL_PASSWORD=<insert password> -e POSTGRESQL_DATABASE="SDB" -e POSTGRESQL_REPLICATION_MODE=slave -e POSTGRESQL_MASTER_HOST=master -e POSTGRESQL_REPLICATION_USER=primary -e POSTGRESQL_REPLICATION_PASSWORD=replication bitnami/postgresql:latest
```

For more information on Bitnami PostgreSQL, please refer to the [bitnami-docker-postgresql](https://github.com/bitnami/bitnami-docker-postgresql) repository.




