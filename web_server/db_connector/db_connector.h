#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <string>
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>

namespace database{
    class Database{
    private:
        std::string _connection_string;
        Database();
    public:
        static Database& get();
        Poco::Data::Session create_session();
        static size_t get_max_shard();
        static std::vector<std::string> get_all_sharding_hint();
        static std::string sharding_hint(std::string login);
    };
}
#endif