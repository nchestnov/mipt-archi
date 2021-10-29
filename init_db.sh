#!/bin/bash
CONTAINER_NAME=archi_mysql_1
export $(cat env_file | sed 's/#.*//g' | xargs)
echo "${DB_ROOT_PASSWORD}"

docker exec -i ${CONTAINER_NAME} mysql --user=root --password="${DB_ROOT_PASSWORD/$'\r'/}" << END

CREATE DATABASE webserver;
GRANT ALL PRIVILEGES ON webserver.* TO 'webserver_admin'@'%';
FLUSH PRIVILEGES;
USE webserver;
CREATE TABLE IF NOT EXISTS Person (
    login VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    first_name VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    last_name VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    age INT NOT NULL,
    PRIMARY KEY (login),
    KEY fn (first_name),
    KEY ln (last_name)
);

END