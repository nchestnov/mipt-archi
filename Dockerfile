FROM ubuntu:latest
EXPOSE 8080
ENV DEBIAN_FRONTEND=noninteractive \
    DB_HOST=127.0.0.1 \
    DB_PORT=3306 \
    DB_NAME=Person \
    DB_LOGIN=root \
    DB_PASSWORD=admin \
    CACHE_SERVERS=''

RUN apt-get update -y && apt-get install -y \
    wget \
    unzip \
    build-essential \
    openjdk-11-jdk \
    unixodbc-dev \
    libssl-dev \
    cmake \
    g++ \
    libpoco-dev \
    && wget -q https://dlcdn.apache.org//ignite/2.11.0/apache-ignite-2.11.0-bin.zip \
    && unzip apache-ignite-2.11.0-bin.zip \
    && mkdir apache-ignite-2.11.0-build && cd apache-ignite-2.11.0-build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DWITH_THIN_CLIENT=ON /apache-ignite-2.11.0-bin/platforms/cpp \
    && make && make install

COPY web_server /archi/web_server

WORKDIR /archi/web_server/build

RUN cmake .. && cmake --build . --target main -- -j 9

CMD /archi/web_server/build/main\
 --db_host=$DB_HOST\
 --db_port=$DB_PORT\
 --db_name=$DB_NAME\
 --db_login=$DB_LOGIN\
 --db_password=$DB_PASSWORD\
 --cache_servers=$CACHE_SERVERS
