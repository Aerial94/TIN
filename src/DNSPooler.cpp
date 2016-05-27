#include "DNSPooler.hpp"
#include <chrono>
#include "Database.hpp"
#include "DNSPacket.hpp"
#include "UDPSocket.hpp"
#include "Logger.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>

DNSPooler::DNSPooler(int interval) {
    this->interval = interval;
    this->rootServers.push_back("198.41.0.4");
    this->rootServers.push_back("192.228.79.201");
    this->rootServers.push_back("192.33.4.12");
    this->rootServers.push_back("199.7.91.13");
    this->rootServers.push_back("192.203.230.10");
    this->rootServers.push_back("192.5.5.241");
    this->rootServers.push_back("192.112.36.4");
    this->rootServers.push_back("198.97.190.53");
    this->rootServers.push_back("192.36.148.17");
    this->rootServers.push_back("192.58.128.30");
    this->rootServers.push_back("193.0.14.129");
    this->rootServers.push_back("199.7.83.42");
    this->rootServers.push_back("202.12.27.33");
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
        Logger::getInstance().logInfo("DNSPooler", "Starting pooling...");
        this->refreshDomains();
        Logger::getInstance().logInfo("DNSPooler", "Pooling done");
        std::chrono::seconds sleepTime(this->interval);
        std::this_thread::sleep_for(sleepTime);
    }
}

void DNSPooler::refreshDomains() {
    std::vector<std::string> domainsNames = Database::getInstance().copy();
    for (auto d = domainsNames.begin(); d != domainsNames.end(); ++d) {
        Logger::getInstance().logInfo("DNSPooler",
                                      "Refreshing domain: " + *d);
        bool mustGo = true;
        std::vector<std::string> dnsServers = this->rootServers;
        while (mustGo) {
            for (auto i = dnsServers.begin(); i != dnsServers.end(); ++i) {
                UDPSocket udpSocket;

                DNSPacket dnsPacket;
                dnsPacket.markAsQuestion();
                DNSQuestion dnsQuestion(*d);
                dnsPacket.addQuestion(dnsQuestion);

                SocketAddress address;
                std::string dnsServer = *i;
                address.setAddress(dnsServer);
                address.setPort(53); // DNS port 53
                udpSocket.setAddress(address);
                udpSocket << dnsPacket;

                DNSPacket recive;
                try {
                    udpSocket >> recive;
                }
                catch (TimeoutException &e) {
                    Logger::getInstance().logWarning("DNSPooler",
                                                     "Timeout while refreshing domain: " +
                                                     *d +
                                                     " using DNS Server: " +
                                                     dnsServer);
                    if (dnsServers.end() - i == 1) {
                        mustGo = false;
                    }
                    continue;
                }
                if (recive.isOk()) {
                    if (recive.getAnswerCount() != 0) {
                        Database::getInstance().updateDomain(*d,
                                                             Domain::FOLLOWED);
                        mustGo = false;
                        break;
                    }
                    else if (recive.getAdditionalCount()) {
                        dnsServers.clear();
                        std::vector<DNSAdditionalRecord> add = recive.getAdditional();
                        for (auto j = add.begin(); j != add.end(); j++) {
                            in_addr addr;
                            addr.s_addr = j->getIP();
                            const char *result = inet_ntoa(addr);
                            dnsServers.push_back(result);
                        }
                        break;
                    }
                    else if (recive.getAuthorityCount()){
                        dnsServers.clear();
                        std::vector<DNSAuthoritativeNameServer> add = recive.getAuthorityNameServers();
                        for (auto j = add.begin(); j != add.end(); j++) {
                            try {
                                std::string ip = hostnameToIP(j->getQName());
                                dnsServers.push_back(ip);
                            }
                            catch (TimeoutException &exception) {
                                continue;
                            }
                        }
                        if (dnsServer.empty())
                        {
                            Database::getInstance().updateDomain(*d,
                                                                 Domain::NONEXISTENT);
                            mustGo = false;
                            break;
                        }
                        break;
                    }
            }
            else {
                Database::getInstance().updateDomain(*d,
                                                     Domain::NONEXISTENT);
                mustGo = false;
                break;
            }
        }
    }
}

}
