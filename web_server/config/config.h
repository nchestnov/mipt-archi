#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
private:
    Config();

    std::string _db_host;
    std::string _db_port;
    std::string _db_login;
    std::string _db_password;
    std::string _db_name;

public:
    static Config &get();

    std::string &db_host();

    std::string &db_port();

    std::string &db_name();

    std::string &db_login();

    std::string &db_password();

    [[nodiscard]] const std::string &get_db_host() const;

    [[nodiscard]] const std::string &get_db_port() const;

    [[nodiscard]] const std::string &get_db_name() const;

    [[nodiscard]] const std::string &get_db_login() const;

    [[nodiscard]] const std::string &get_db_password() const;
};

#endif
