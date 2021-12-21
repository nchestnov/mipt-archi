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

    public:
        //static Person fromJSON(const Poco::JSON::Object::Ptr &object);
        static Person fromJSON(const std::string &json);

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

        static std::vector<Person> searchByNames(std::string first_name, std::string last_name);

        void db_save();

        void db_delete();

        void queue_save();

        Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif
