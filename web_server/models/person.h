#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"

namespace database {
    class Person {
    private:
        std::string _login;
        std::string _first_name;
        std::string _last_name;
        int _age;

        static std::vector<Person> searchByNames_shard(std::string first_name, std::string last_name, std::string hint);

    public:
        static Person fromJSON(const Poco::JSON::Object::Ptr &object);

        const std::string &get_login() const;

        const std::string &get_first_name() const;

        const std::string &get_last_name() const;

        const int &get_age() const;

        std::string &login();

        std::string &first_name();

        std::string &last_name();

        int &age();

        bool check();

        static Person findByLogin(std::string login);

        static std::vector<Person> searchByNames(const std::string& first_name, std::string last_name);

        void db_save();

        void db_delete();

        Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif
