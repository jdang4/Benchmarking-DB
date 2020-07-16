# Benchmarking DB Application


# Before Running

This application is designed to run in an individual docker container and connects with the running database servers that are all in their own docker containers as well. So before running this application, it is required that the containers that hold the database server are up and running.


## How to Deploy a Docker Container with a Database Server

### Redis

My reccomendation is to get the Bitnami Redis Docker Images is to pull the prebuild image from the Docker Hub Registry.


```console
$ docker pull bitnami/redis:latest
```

After pulling the prebuilt image you can go ahead and get the Redis container running in the Docker Engine. The following command is what I use that includes my own Redis config file, persistence, and replication. Please note that the filepaths included in my commands must be changed to fit with your system.


Redis Primary Container:

```console
$ docker run --name redis-master -v ~/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data -e REDIS_REPLICATION_MODE=master -e ALLOW_EMPTY_PASSWORD=yes --rm -d bitnami/redis:latest
```


Redis Replica Container:

```console
$ docker run --name redis-replica -v /home/jonathand/docker/Benchmark/config/redis.conf:/opt/bitnami/redis/mounted-etc/redis.conf -v /var/lib/redis:/bitnami/redis/data --link redis-master:master -e REDIS_REPLICATION_MODE=slave -e REDIS_MASTER_HOST=master -e ALLOW_EMPTY_PASSWORD=yes --rm -d bitnami/redis:latest
```





