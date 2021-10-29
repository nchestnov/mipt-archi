# MIPT Software Architecture

This is a course project repository for Software Architechture course at MIPT. 

Main Task - create REST API.

## Files

- web_server/ - source code of web server based on Poco library
- docker-compose.yml - Docker Compose file for deploying server
- Dockerfile - Dockerfile for building web server
- env_file - file with environment variables for deployment
- init.db - script for database initialization

## How to use?

1. Build Docker container:
    
    `docker build . -t archi`

2. Deploy Docker Compose:
   
   `docker-compose up -d`

3. Wait a minute for MySQL server to fully deploy. Then run initialization script:
   
   `./init_db.sh`