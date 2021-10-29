#include "http_web_server.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    HTTPWebServer app;
    return app.run(argc, argv);
}