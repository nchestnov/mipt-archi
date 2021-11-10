#include "person.h"
#include "../db_connector/db_connector.h"
#include "../db_connector/cache.h"
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include "Poco/Logger.h"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Data::RecordSet;
using Poco::Logger;

namespace database {
    Poco::JSON::Object::Ptr Person::toJSON() const {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("login", _login);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("age", _age);

        return root;
    }

    Person Person::fromJSON(const std::string &str) {
        Person person;

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        person.login() = object->getValue<std::string>("login");
        person.first_name() = object->getValue<std::string>("first_name");
        person.last_name() = object->getValue<std::string>("last_name");
        person.age() = object->getValue<int>("age");

        return person;
    }

    Person Person::db_get_by_login(std::string login) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Person person;
            select << "SELECT login, first_name, last_name, age FROM Person where login=?",
                    into(person._login),
                    into(person._first_name),
                    into(person._last_name),
                    into(person._age),
                    use(login),
                    range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst()) person._not_found = true;
            return person;
        }

        catch (Poco::Data::MySQL::ConnectionException &err) {
            std::string err_string = err.what();
            Logger::root().error("MySQL Connection Error: " + err_string);
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &err) {
            std::string err_string = err.what();
            Logger::root().error("MySQL Statement Error: " + err_string);
            throw;
        }
    }

    Person Person::cache_get_by_login(std::string login) {
        try {
            std::string result;
            if (database::Cache::get().get(login, result))
                return fromJSON(result);
            else
                throw std::logic_error("key not found in the cache");
        } catch (std::exception& err) {
            std::string err_string = err.what();
            Logger::root().error("cache_get_by_login: " + err_string);
            throw;
        }
    }

    std::vector<Person> Person::searchByNames(std::string first_name, std::string last_name) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Person> result;
            Person person;
            first_name += "%";
            last_name += "%";

            select
                    << "SELECT login, first_name, last_name, age FROM Person WHERE first_name LIKE ? AND last_name LIKE ?",
                    into(person._login),
                    into(person._first_name),
                    into(person._last_name),
                    into(person._age),
                    use(first_name),
                    use(last_name),
                    range(0, 1);

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) result.push_back(person);

            while (!select.done()) {
                select.execute();
                result.push_back(person);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &err) {
            std::string err_string = err.what();
            Logger::root().error("MySQL Connection Error: " + err_string);
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &err) {
            std::string err_string = err.what();
            Logger::root().error("MySQL Statement Error: " + err_string);
            throw;
        }
    }

    void Person::db_save() {

        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Person (login, first_name, last_name, age) VALUES(?, ?, ?, ?)",
                    use(_login),
                    use(_first_name),
                    use(_last_name),
                    use(_age);

            insert.execute();
        }
        catch (Poco::Data::MySQL::ConnectionException &err) {
            std::string err_string = err.what();
            Logger::root().error("MySQL Connection Error: " + err_string);
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &err) {
            std::string err_string = err.what();
            Logger::root().error("MySQL Statement Error: " + err_string);
            throw;
        }
    }

    void Person::cache_save() {
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(toJSON(), ss);
        std::string message = ss.str();
        database::Cache::get().put(_login, message);
    }

    void Person::db_delete() {

        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "DELETE FROM Person WHERE login = ?",
                    use(_login);

            insert.execute();
        }
        catch (Poco::Data::MySQL::ConnectionException &err) {
            std::string err_string = err.what();
            Logger::root().error("MySQL Connection Error: " + err_string);
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &err) {
            std::string err_string = err.what();
            Logger::root().error("MySQL Statement Error: " + err_string);
            throw;
        }
    }

    const std::string &Person::get_login() const {
        return _login;
    }

    const std::string &Person::get_first_name() const {
        return _first_name;
    }

    const std::string &Person::get_last_name() const {
        return _last_name;
    }

    const int &Person::get_age() const {
        return _age;
    }

    std::string &Person::login() {
        return _login;
    }

    std::string &Person::first_name() {
        return _first_name;
    }

    std::string &Person::last_name() {
        return _last_name;
    }

    int &Person::age() {
        return _age;
    }

    bool Person::check() {
        //return !(_first_name.empty() && _last_name.empty() && _login.empty());
        return !_not_found;
    }
}