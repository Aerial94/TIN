#include "DNSPooler.hpp"
#include <chrono>

DNSPooler::DNSPooler(int interval) {
    this->interval = interval;
}

void DNSPooler::run() {
    std::thread thread(&DNSPooler::pool, this);
    thread.detach();
}

void DNSPooler::setInterval(int interval) {
    this->interval = interval;
}

void DNSPooler::pool() {
    while (true) {
        this->refreshDomains();
        std::chrono::seconds sleepTime(this->interval);
        std::this_thread::sleep_for(sleepTime);
    }
}

void DNSPooler::refreshDomains() {

}









