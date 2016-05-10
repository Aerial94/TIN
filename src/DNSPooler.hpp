#ifndef DNS_CHECKER_DNSPOOLER_HPP
#define DNS_CHECKER_DNSPOOLER_HPP

#include <thread>

class DNSPooler {
    static const std::string rootServers[];
    int interval;
    void pool();
    void refreshDomains();
public:
    DNSPooler(int interval);
    void run();
    void setInterval(int interval);

};


#endif //DNS_CHECKER_DNSPOOLER_HPP
