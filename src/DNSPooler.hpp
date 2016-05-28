#ifndef DNS_CHECKER_DNSPOOLER_HPP
#define DNS_CHECKER_DNSPOOLER_HPP

#include <thread>
#include <vector>
#include <atomic>

class DNSPooler {
    int interval;
    void pool();
    void refreshDomains();
    std::vector<std::string> rootServers;
public:
    DNSPooler(int interval);
    void run();
    void setInterval(int interval);

    void stop();

    std::thread thread;
    std::atomic_bool stopThread;
};


#endif //DNS_CHECKER_DNSPOOLER_HPP
