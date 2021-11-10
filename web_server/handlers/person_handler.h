#ifndef PERSON_HANDLER_H
#define PERSON_HANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/JSON/Object.h"
#include <Poco/JSON/Parser.h>
#include <Poco/StreamCopier.h>
#include <iostream>
#include <fstream>
#include <utility>

#include "Poco/Logger.h"

using Poco::Logger;

using Poco::ThreadPool;
using Poco::StreamCopier;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

#include "../models/person.h"

class PersonRequestHandler : public HTTPRequestHandler {
public:
    explicit PersonRequestHandler(std::string format) : _format(std::move(format)) {
    }

    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) override {
        Poco::Net::HTMLForm form(request);

        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");

        bool no_cache = false;

        if (request.getMethod() == "GET") {
            if (form.has("login")) {
                std::string login = form.get("login");
                Logger::root().information("GET /person?login="+login);

                if (form.has("no_cache"))
                    no_cache = true;

                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

                if (!no_cache)
                {
                    try {
                        database::Person cache_result = database::Person::cache_get_by_login(login);
                        Logger::root().information("Cache found for login: " + login);
                        root->set("login", cache_result.get_login());
                        root->set("first_name", cache_result.get_first_name());
                        root->set("last_name", cache_result.get_last_name());
                        root->set("age", cache_result.get_age());
                    } catch (...) {
                        Logger::root().information("Cache not found for login: " + login);
                        database::Person db_result = database::Person::db_get_by_login(login);

                        if (db_result.check()) {
                            root->set("login", db_result.get_login());
                            root->set("first_name", db_result.get_first_name());
                            root->set("last_name", db_result.get_last_name());
                            root->set("age", db_result.get_age());
                            db_result.cache_save();
                        } else {
                            root->set("status", "person_not_found");
                            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                        }
                    }
                } else {
                    Logger::root().information("Ignored cache for login: " + login);
                    database::Person db_result = database::Person::db_get_by_login(login);

                    if (db_result.check()) {
                        root->set("login", db_result.get_login());
                        root->set("first_name", db_result.get_first_name());
                        root->set("last_name", db_result.get_last_name());
                        root->set("age", db_result.get_age());
                    } else {
                        root->set("status", "person_not_found");
                        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    }
                }

                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);

            } else if (form.has("first_name") && form.has("last_name")) {
                std::string first_name = form.get("first_name");
                std::string last_name = form.get("last_name");
                Logger::root().information("GET /person?first_name="+first_name+"&last_name="+last_name);

                Poco::JSON::Array arr;
                std::vector<database::Person> result = database::Person::searchByNames(first_name, last_name);

                for (auto &i: result) {
                    arr.add(i.toJSON());
                }
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(arr, ostr);
            } else {
                response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                response.send();
            }
        } else if (request.getMethod() == "POST") {
            Logger::root().information("POST /person");
            std::ostringstream out_string_stream;
            Poco::StreamCopier::copyStream(request.stream(), out_string_stream);
            std::string raw_json = out_string_stream.str();
            Poco::JSON::Parser parser;
            Poco::JSON::Object::Ptr json = parser.parse(raw_json).extract<Poco::JSON::Object::Ptr>();

            if (json->has("login") && json->has("first_name") && json->has("last_name") && json->has("age")){
                database::Person new_person;

                new_person.login() = json->getValue<std::string>("login");
                new_person.first_name() = json->getValue<std::string>("first_name");
                new_person.last_name() = json->getValue<std::string>("last_name");
                new_person.age() = json->getValue<int>("age");
                database::Person existing_person = database::Person::db_get_by_login(new_person.get_login());
                if (existing_person.check()) {
                    response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_CONFLICT);
                } else {
                    new_person.db_save();
                    new_person.cache_save();
                    response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_CREATED);
                }
            } else {
                response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
            }
        } else {
            Logger::root().warning("Bad Request");
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        }
        response.send();
    }

private:
    std::string _format;
};

#endif //PERSON_HANDLER_H
