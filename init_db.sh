#!/bin/bash
CONTAINER_NAME=archi_mysql_1
export $(cat env_file | sed 's/#.*//g' | xargs)

docker exec -i ${CONTAINER_NAME} mysql --user=root --password="${DB_ROOT_PASSWORD/$'\r'/}" << END

CREATE DATABASE webserver;
GRANT ALL PRIVILEGES ON webserver.* TO 'webserver_admin'@'%';
FLUSH PRIVILEGES;
USE ${DB_NAME/$'\r'/};
CREATE TABLE IF NOT EXISTS Person (
    login VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    first_name VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    last_name VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    age INT NOT NULL,
    PRIMARY KEY (login),
    INDEX name_index (first_name,last_name)
);

END

for i in `awk -F, '{if (NR>1) {print "{\"login\":\"" $1 "\",\"first_name\":\"" $2 "\",\"last_name\":\"" $3 "\",\"age\":" $4 "}"}}' mock_data.csv`; do
   curl --header "Content-Type: application/json" --request POST -s "http://localhost:8080/person" -d $i
done