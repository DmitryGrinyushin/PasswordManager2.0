#include "WebServer.h"
#include <iostream>

int main() {
    std::string host = "0.0.0.0";
    int port = 8080;

    WebServer server(host, port);
    std::cout << "Starting web server on " << host << ":" << port << std::endl;
    server.start();

    return 0;
}