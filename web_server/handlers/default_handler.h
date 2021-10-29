#ifndef DEFAULT_HANDLER_H
#define DEFAULT_HANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
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
#include "Poco/JSON/Object.h"
#include <Poco/JSON/Parser.h>
#include <Poco/StreamCopier.h>
#include <iostream>
#include <iostream>
#include <fstream>
#include <utility>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::StreamCopier;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

class DefaultRequestHandler : public HTTPRequestHandler {
public:
    explicit DefaultRequestHandler(std::string format) : _format(std::move(format)) {
    }

    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) override {
        Poco::Net::HTMLForm form(request);

        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.send();
    }

private:
    std::string _format;
};

#endif //DEFAULT_HANDLER_H
