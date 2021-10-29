#include "config.h"

Config::Config()
= default;

Config &Config::get() {
    static Config _instance;
    return _instance;
}

const std::string &Config::get_db_host() const {
    return _db_host;
}

const std::string &Config::get_db_port() const {
    return _db_port;
}

const std::string &Config::get_db_name() const {
    return _db_name;
}

const std::string &Config::get_db_login() const {
    return _db_login;
}

const std::string &Config::get_db_password() const {
    return _db_password;
}

std::string &Config::db_port() {
    return _db_port;
}

std::string &Config::db_host() {
    return _db_host;
}

std::string &Config::db_login() {
    return _db_login;
}

std::string &Config::db_password() {
    return _db_password;
}

std::string &Config::db_name() {
    return _db_name;
}