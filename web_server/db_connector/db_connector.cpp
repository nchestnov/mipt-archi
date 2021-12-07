#include "db_connector.h"
#include "../config/config.h"

namespace database{
    Database::Database(){
        _connection_string+="host=";
        _connection_string+=Config::get().get_db_host();
        _connection_string+=";port=";
        _connection_string+=Config::get().get_db_port();
        _connection_string+=";user=";
        _connection_string+=Config::get().get_db_login();
        _connection_string+=";db=";
        _connection_string+=Config::get().get_db_name();
        _connection_string+=";password=";
        _connection_string+=Config::get().get_db_password();

        Poco::Data::MySQL::Connector::registerConnector();
    }

    Database& Database::get(){
        static Database _instance;
        return _instance;
    }

    Poco::Data::Session Database::create_session(){
        return Poco::Data::Session(Poco::Data::SessionFactory::instance().create(Poco::Data::MySQL::Connector::KEY, _connection_string));
    }

    size_t Database::get_max_shard(){
        return 2;
    }

    std::vector<std::string> Database::get_all_sharding_hint(){
        std::vector<std::string> result;
        for(size_t i=0;i<get_max_shard();++i){
            std::string shard_name = "-- sharding:";
            shard_name += std::to_string(i);
            result.push_back(shard_name);
        }
        return result;
    }

    std::string Database::sharding_hint(std::string login){
        size_t shard_number = std::hash<std::string>{}(login)%get_max_shard();
        std::string result = "-- sharding:" + std::to_string(shard_number);
        return result;
    }
}