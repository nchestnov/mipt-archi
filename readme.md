# MIPT Software Architecture

This is a course project repository for Software Architechture course at MIPT. 

Main Task - create REST API.

## Files

- web_server/ - source code of web server based on Poco library
- Architect.postman_collection.json - Postman Collection of request for testing
- docker-compose.yml - Docker Compose file for deploying server
- Dockerfile - Dockerfile for building web server
- env_file - file with environment variables for deployment
- init.db - script for database initialization

## How to use?

1. Build Docker container:
    
    `docker build Dockerfile_webserver -t zetman13/archi:task4_webserver` 

    `docker build Dockerfile_worker -t zetman13/archi:task4_worker`

   or pull existing image:

   `docker pull zetman13/archi:task4_webserver`

   `docker pull zetman13/archi:task4_worker`

3. Deploy Docker Compose with env_file:
   
   `docker-compose --env-file env_file up -d`

4. Wait a minute for MySQL server to fully deploy. Then run initialization script to init table and fill with initial data (it will take some time - approx. 1-2 minute):
   
   `./init_db.sh`