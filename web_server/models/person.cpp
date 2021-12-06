#include "person.h"
#include "../db_connector/db_connector.h"
#include <Poco/Data/RecordSet.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Data::RecordSet;

namespace database {
    Poco::JSON::Object::Ptr Person::toJSON() const {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("login", _login);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("age", _age);
        return root;
    }

    Person Person::fromJSON(const Poco::JSON::Object::Ptr &object) {
        Person person;
        person.login() = object->getValue<std::string>("login");
        person.first_name() = object->getValue<std::string>("first_name");
        person.last_name() = object->getValue<std::string>("last_name");
        person.age() = object->getValue<int>("age");
        return person;
    }

    Person Person::findByLogin(std::string login) {
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
            if (!rs.moveFirst()){
                throw Poco::Data::NoDataException();
            } else {
                return person;
            }
        }

        catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e) {

            std::cout << "statement:" << e.what() << std::endl;
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

        catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e) {

            std::cout << "statement:" << e.what() << std::endl;
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
        catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e) {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Person::db_delete() {

        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "DELETE FROM Person WHERE login = ?",
                    use(_login);

            insert.execute();
        }
        catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e) {

            std::cout << "statement:" << e.what() << std::endl;
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
}