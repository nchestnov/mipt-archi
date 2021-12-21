//
// Created by Nikita on 27.10.2021.
//

#ifndef HTTPWEBSERVER_H
#define HTTPWEBSERVER_H

#include <string>
#include <iostream>
#include <fstream>

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include "Poco/ConsoleChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/Logger.h"
#include "Poco/AutoPtr.h"
using Poco::ConsoleChannel;
using Poco::FormattingChannel;
using Poco::PatternFormatter;
using Poco::Logger;
using Poco::AutoPtr;

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "http_request_factory.h"
#include "config/config.h"

class HTTPWebServer : public Poco::Util::ServerApplication
{
public:
    HTTPWebServer() : _helpRequested(false)
    {
    }

    ~HTTPWebServer() override = default;

protected:
    void initialize(Application &self) override
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize() override
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet &options) override
    {
        ServerApplication::defineOptions(options);

        options.addOption(
                Option("help", "h", "display argument help information")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleHelp)));

        options.addOption(
                Option("db_host", "", "Database host or IP address")
                .required(true)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleDBHost)));

        options.addOption(
                Option("db_port", "", "Database port")
                .required(true)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleDBPort)));

        options.addOption(
                Option("db_name", "", "Database name")
                .required(true)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleDBName)));

        options.addOption(
                Option("db_login", "", "Database login")
                .required(true)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleDBLogin)));

        options.addOption(
                Option("db_password", "", "Database password")
                .required(true)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleDBPassword)));

        options.addOption(
                Option("queue_host", "", "Queue host")
                .required(true)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleQueueHost)));

        options.addOption(
                Option("queue_topic", "", "Queue topic")
                .required(true)
                .repeatable(false)
                .argument("value")
                .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::handleQueueTopic)));
    }

    void handleHelp([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value){
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A simple API web server");
        helpFormatter.format(std::cout);
        stopOptionsProcessing();
        _helpRequested = true;
    }

    void handleDBHost([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value){
        Config::get().db_host() = value;
    }

    void handleDBPort([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value){
        Config::get().db_port() = value;
    }

    void handleDBName([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value){
        Config::get().db_name() = value;
    }

    void handleDBLogin([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value){
        Config::get().db_login() = value;
    }

    void handleDBPassword([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value){
        Config::get().db_password() = value;
    }

    void handleQueueHost([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value){
        Config::get().queue_host() = value;
    }

    void handleQueueTopic([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value){
        Config::get().queue_topic() = value;
    }

    int main([[maybe_unused]] const std::vector<std::string> &args) override{
        if (!_helpRequested){
            unsigned short port = (unsigned short) config().getInt("HTTPWebServer.port", 8080);
            std::string format(config().getString("HTTPWebServer.format", DateTimeFormat::SORTABLE_FORMAT));

            ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", port));
            HTTPServer srv(new HTTPRequestFactory(format), svs, new HTTPServerParams);

            AutoPtr<ConsoleChannel> pCons(new ConsoleChannel);
            AutoPtr<PatternFormatter> pPF(new PatternFormatter);
            pPF->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s,%T:(%p) %t");
            AutoPtr<FormattingChannel> pFC(new FormattingChannel(pPF, pCons));
            Logger::root().setChannel(pFC);

            Logger::root().information("Started server on port: 8080");
            srv.start();

            waitForTerminationRequest();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};


#endif //HTTPWEBSERVER_H
