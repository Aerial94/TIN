#include "DNSPooler.hpp"
#include "DNSPacket.hpp"
#include "Database.hpp"
#include "Logger.hpp"
#include "UDPSocket.hpp"
#include <arpa/inet.h>
#include <chrono>
#include <netinet/in.h>

DNSPooler::DNSPooler(int interval) {
    this->interval = interval;
}

void DNSPooler::run() {
    this->stopThread = false;
    this->thread = std::thread(&DNSPooler::pool, this);
}

void DNSPooler::setInterval(int interval) {
    this->interval = interval;
}

void DNSPooler::pool() {
    int sleepCounter = this->interval;
    while (not this->stopThread) {
        if (sleepCounter == this->interval) {
            Logger::getInstance().logInfo("DNSPooler", "Starting pooling...");
            this->refreshDomains();
            Logger::getInstance().logInfo("DNSPooler", "Pooling done");
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        sleepCounter--;
        if (sleepCounter <= 0)
            sleepCounter = this->interval;
    }
}

void DNSPooler::refreshDomains() {
    std::vector<Domain> domainsNames = Database::getInstance().copy();
    for (auto d = domainsNames.begin(); d != domainsNames.end(); ++d) {
        Logger::getInstance().logInfo("DNSPooler", "Refreshing domain: " +
                                                       d->getDomainName());
        int depth = 0;
        bool mustGo = true;
        std::vector<std::string> dnsServers = this->rootServers;
        while (mustGo) {
            for (auto i = dnsServers.begin(); i != dnsServers.end(); ++i) {
                UDPSocket udpSocket;

                DNSPacket dnsPacket;
                dnsPacket.markAsQuestion();
                DNSQuestion dnsQuestion(d->getDomainName());
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
                } catch (TimeoutException &e) {
                    Logger::getInstance().logWarning(
                        "DNSPooler", "Timeout while refreshing domain: " +
                                         d->getDomainName() +
                                         " using DNS Server: " + dnsServer);
                    if (dnsServers.end() - i == 1) {
                        mustGo = false;
                    }
                    continue;
                }
                if (recive.isOk() and depth < 128) {
                    if (recive.getAnswerCount() != 0) {
                        std::vector<DNSAnswer> answers = recive.getAnswers();
                        DNSAnswer answer = answers[0];
                        in_addr addresStruct;
                        addresStruct.s_addr = answer.getIP();
                        const char *result = inet_ntoa(addresStruct);
                        std::string ipString = result;
                        Database::getInstance().updateDomain(d->getDomainName(),
                                                             ipString);
                        mustGo = false;
                        break;
                    }
                    else if (recive.getAdditionalCount()) {
                        dnsServers.clear();
                        std::vector<DNSAdditionalRecord> add =
                            recive.getAdditional();
                        for (auto j = add.begin(); j != add.end(); j++) {
                            in_addr addr;
                            addr.s_addr = j->getIP();
                            const char *result = inet_ntoa(addr);
                            dnsServers.push_back(result);
                        }
                        break;
                    }
                    else if (recive.getAuthorityCount()) {
                        dnsServers.clear();
                        std::vector<DNSAuthoritativeNameServer> add =
                            recive.getAuthorityNameServers();
                        for (auto j = add.begin(); j != add.end(); j++) {
                            try {
                                std::string ip = hostnameToIP(j->getQName());
                                dnsServers.push_back(ip);
                            } catch (TimeoutException &exception) {
                                continue;
                            }
                        }
                        if (dnsServer.empty()) {
                            Database::getInstance().updateDomain(
                                d->getDomainName(), Domain::NONEXISTENT);
                            mustGo = false;
                            break;
                        }
                        break;
                    }
                    depth++;
                }
                else {
                    Database::getInstance().updateDomain(d->getDomainName(),
                                                         Domain::NONEXISTENT);
                    mustGo = false;
                    break;
                }
            }
        }
    }
}

void DNSPooler::stop() {
    Logger::getInstance().logInfo("DNSPooler", "Stopping DNSPooler...");
    this->stopThread = true;
    this->thread.join();
}
