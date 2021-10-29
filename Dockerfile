FROM ubuntu:latest
EXPOSE 8080
ENV DEBIAN_FRONTEND=noninteractive \
    DB_HOST=127.0.0.1 \
    DB_PORT=3306 \
    DB_NAME=Person \
    DB_LOGIN=root \
    DB_PASSWORD=admin

RUN apt-get update -y && apt-get install -y \
    cmake \
    g++ \
    libpoco-dev

COPY web_server /archi/web_server

WORKDIR /archi/web_server/build

RUN cmake .. && cmake --build . --target main -- -j 9

CMD /archi/web_server/build/main\
 --db_host=$DB_HOST\
 --db_port=$DB_PORT\
 --db_name=$DB_NAME\
 --db_login=$DB_LOGIN\
 --db_password=$DB_PASSWORD
