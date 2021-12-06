#ifndef PERSON_HANDLER_H
#define PERSON_HANDLER_H

#include <Poco/Data/RecordSet.h>
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
using Poco::Data::RecordSet;
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

    void get_person_by_login(Poco::Net::HTMLForm &form, HTTPServerResponse &response){
        std::string login = form.get("login");

        std::ostream &ostr = response.send();
        try{
            database::Person person = database::Person::findByLogin(login);
            Poco::JSON::Stringifier::stringify(person.toJSON(), ostr);
        } catch(Poco::Data::NoDataException&) {
            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
            root->set("status", "person_not_found");
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
            Poco::JSON::Stringifier::stringify(root, ostr);
        }

        Logger::root().information("GET /person?login="+login);
    }

    void get_person_by_names(Poco::Net::HTMLForm &form, HTTPServerResponse &response){
        std::string first_name = form.get("first_name");
        std::string last_name = form.get("last_name");

        Poco::JSON::Array arr;
        std::vector<database::Person> result = database::Person::searchByNames(first_name, last_name);

        for (auto &i: result) {
            arr.add(i.toJSON());
        }
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(arr, ostr);

        Logger::root().information("GET /person?first_name="+first_name+"&last_name="+last_name);
    }

    bool validate_json(Poco::JSON::Object::Ptr &json){
        return json->has("login") && json->has("first_name") && json->has("last_name") && json->has("age");
    }

    void post_person(Poco::JSON::Object::Ptr &json, HTTPServerResponse &response){
        database::Person new_person = database::Person::fromJSON(json);
        try {
            database::Person existing_person = database::Person::findByLogin(new_person.get_login());
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_CONFLICT);
        } catch(Poco::Data::NoDataException&) {
            new_person.db_save();
            response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_CREATED);
        }

        Logger::root().information("POST /person");
    }

    void bad_request(HTTPServerResponse &response){
        Logger::root().warning("Bad Request");
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    }

    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) override {
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");

        if (request.getMethod() == "GET") {
            Poco::Net::HTMLForm form(request);

            if (form.has("login")) {
                get_person_by_login(form, response);
            } else if (form.has("first_name") && form.has("last_name")) {
                get_person_by_names(form, response);
            } else {
                bad_request(response);
            }
        } else if (request.getMethod() == "POST") {
            std::ostringstream out_string_stream;
            Poco::StreamCopier::copyStream(request.stream(), out_string_stream);
            std::string raw_json = out_string_stream.str();
            Poco::JSON::Parser parser;
            Poco::JSON::Object::Ptr json = parser.parse(raw_json).extract<Poco::JSON::Object::Ptr>();

            if (validate_json(json)){
                post_person(json, response);
            } else {
                bad_request(response);
            }
        } else {
            bad_request(response);
        }
        response.send();
    }

private:
    std::string _format;
};

#endif //PERSON_HANDLER_H
