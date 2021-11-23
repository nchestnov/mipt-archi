# MIPT Software Architecture

This is a course project repository for Software Architechture course at MIPT. 

Main Task - create REST API. Now with caching (powered by Apache Ignite).

## Files

- web_server/ - source code of web server based on Poco library
- Architect.postman_collection.json - Postman Collection of request for testing
- docker-compose.yml - Docker Compose file for deploying server
- Dockerfile - Dockerfile for building web server
- env_file - file with environment variables for deployment
- init.db - script for database initialization

## How to use?

1. Build Docker image:
    
    `docker build . -t zetman13/archi:task2`

   or pull existing image:

   `docker pull zetman13/archi:task2`

3. Deploy Docker Compose with env_file:
   
   `docker-compose --env-file env_file up -d`

4. Wait a minute for MySQL server to fully deploy. Then run initialization script to init table and fill with initial data:
   
   `./init_db.sh`

>**Note:** 
>By default initial data is not cached (no cache warm-up). However, when requested login is not found in cache, it is cached. So first GET by login request may be slow, but second will be faster.


## Load Test

### Without Cache

**Command** - `wrk -c 10 -d 30 -t 10 "http://localhost:8080/person?login=nsecretgz&no_cache=1"`

| Thread Stats | Avg | Stdev | Max | +/- Stdev |
| - | - | - | - | - |
| Latency | 38.48ms | 24.81ms | 458.29ms | 95.75% |
| Req/Sec | 27.45 | 6.71 | 70.00 | 60.85% |

**8186** requests in **30.08s**, **1.75MB** read. 

**Socket errors:** connect 0, read 8186, write 0, timeout 0.

Requests/sec:    272.14

Transfer/sec:     59.54KB

### With Cache

**Command** - `wrk -c 10 -d 30 -t 10 "http://localhost:8080/person?login=nsecretgz"`

| Thread Stats | Avg | Stdev | Max | +/- Stdev |
| - | - | - | - | - |
| Latency | 8.22ms | 25.36ms | 647.42ms | 98.82% |
| Req/Sec | 157.89 | 26.31 | 424.00 | 75.39% |

**46921** requests in **30.09s**, **10.02MB** read.

**Socket errors:** connect 0, read 46919, write 0, timeout 0.

**Requests/sec:** 1559.13

**Transfer/sec:** 341.09KB
