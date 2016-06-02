#ifndef DNS_CHECKER_HTTPSERVER_H
#define DNS_CHECKER_HTTPSERVER_H

#include "HTTPHandler.hpp"
#include "Configuration.hpp"
#include "TCPSocket.hpp"
#include <list>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

class Semaphore {
public:

    Semaphore() { this->count = 0; }

    Semaphore(int count) { this->count = count; }

    void notify() {
        std::unique_lock<std::mutex> lck(mtx);
        ++count;
        cv.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lck(mtx);
        while (count == 0) {
            cv.wait(lck);
        }

        --count;
    }

    void setCount(int count) {
        this->count = count;
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};


class HTTPServer {
public:
    HTTPServer();

    void listen();

    void stop();

    void run();

private:
    Semaphore semaphore;
    const char* invalid_request = "Server: dns-checker\r\n"
            "Connection: Closed\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

    const char * valid_request = "Server: dns-checker\r\n"
            "Connection: Closed\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: {0}\r\n";
    TCPSocket socket;

    void response(int clientSocket);

    std::string valid_request_function(std::string &response_json);

    std::string invalid_request_function();

    std::thread thread;
    std::atomic_bool stopThread;
};


#endif //DNS_CHECKER_HTTPSERVER_H
