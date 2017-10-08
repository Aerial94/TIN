#include "HTTPServer.hpp"
#include "HTTPPacket.hpp"
#include <functional>
#include <sstream>
#include <stdlib.h>
#include <thread>

HTTPServer::HTTPServer() {
    Configuration configuration = Configuration::getInstance();
    SocketAddress address;
    if (not address.setAddress(configuration.getHttpServerAddress())) {
        Logger::getInstance().logInfo("Server",
                                      "Cant listen on address: '" +
                                          configuration.getHttpServerAddress() +
                                          "' beacause it seems to be invalid");
        std::cout << "Cant listen on address '" +
                         configuration.getHttpServerAddress() +
                         "' beacause it seems to be invalid\n";
        std::exit(-1);
    }
    address.setPort(configuration.getHttpServerPort());
    this->socket.setReuseAddress();
    if (this->socket.bind(address) < 0) {
        Logger::getInstance().logInfo(
            "Server", "Cant bind address: " +
                          configuration.getHttpServerAddress() + ":" +
                          std::to_string(configuration.getHttpServerPort()));
        std::cout << "Cant bind address " +
                         configuration.getHttpServerAddress() + ":" +
                         std::to_string(configuration.getHttpServerPort()) +
                         "\n";
        std::exit(-1);
    }
    this->socket.listen(configuration.getHttpServerMaxThreads());
    this->semaphore.setCount(configuration.getHttpServerMaxThreads());
}

void HTTPServer::listen() {
    this->socket.setTimeout(1, 0);
    Logger::getInstance().logInfo("Server", "Starting listening...");
    while (not this->stopThread) {
        try {
            int clientSocket = this->socket.accept();
            this->semaphore.wait();
            std::thread thread(&HTTPServer::response, this, clientSocket);
            thread.detach();
        } catch (TimeoutException &e) {
            continue;
        }
    }
}

void HTTPServer::response(int clientSocket) {
    Logger::getInstance().logDebug("Server", "Getting data...");
    TCPSocket tcpSocket(clientSocket);
    tcpSocket.setTimeout(
        Configuration::getInstance().getHttpServerReadTimeout(), 0);
    HTTPPacket packet;
    std::string json_response, response;
    std::string line, json;
    while (true) {
        try {
            line = tcpSocket.readLine();
        } catch (TimeoutException &e) {
            Logger::getInstance().logWarning(
                "Server",
                "Timeout while reciving http packet headers from client");
            this->semaphore.notify();
            return;
        }
        if (line == "\r\n")
            break;
        packet.save_line(line);
    }
    if (packet.is_valid_request()) {
        Logger::getInstance().logDebug("Server", "Valid request");
        try {
            json = tcpSocket.read_from_socket(packet.get_content_length());
        } catch (TimeoutException &e) {
            Logger::getInstance().logWarning(
                "Server",
                "Timeout while reciving http packet json body from client");
            this->semaphore.notify();
            return;
        }
        packet.save_json(json);
        HTTPHandler handler;
        try {
            json_response = handler.getResponse(json);
            response = valid_request_function(json_response);
        } catch (HTTPHandlerException &e) {
            Logger::getInstance().logWarning("Server", "Invalid json: " +
                                                           e.getDescription());
            response = invalid_request_function();
        }
    }
    else {
        Logger::getInstance().logWarning("Server", "Invalid request");
        response = invalid_request_function();
    }
    tcpSocket << response;
    this->semaphore.notify();
}

std::string HTTPServer::valid_request_function(std::string &response_json) {
    std::string response = this->valid_request;
    char buf[1000];
    time_t now = time(0);
    struct tm *tm = gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", tm);
    response.insert(0, "\r\n");
    response.insert(0, buf);
    response.insert(0, "Date: ");
    response.insert(0, "HTTP/1.1 200 OK\r\n");
    size_t f = response.find("{0}");
    response.replace(f, std::string("{0}").length(),
                     std::to_string(response_json.size()));
    response = response + "\r\n" + response_json;
    return response;
}

std::string HTTPServer::invalid_request_function() {
    std::string response = this->invalid_request;
    char buf[1000];
    time_t now = time(0);
    struct tm *tm = gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", tm);
    response.insert(0, "\r\n");
    response.insert(0, buf);
    response.insert(0, "Date: ");
    response.insert(0, "HTTP/1.1 418 I'm teapot\r\n");
    return response;
}

void HTTPServer::stop() {
    Logger::getInstance().logInfo("DNSPooler", "Stopping HTTPServer...");
    this->stopThread = true;
    this->thread.join();
}

void HTTPServer::run() {
    this->stopThread = false;
    this->thread = std::thread(&HTTPServer::listen, this);
}
