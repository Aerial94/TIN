#include "DNSPooler.hpp"
#include <chrono>
#include "Database.hpp"
#include "DNSPacket.hpp"
#include "UDPSocket.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>

DNSPooler::DNSPooler(int interval) {
    this->interval = interval;
    this->rootServers.push_back("192.33.4.12");
    this->rootServers.push_back("192.228.79.201");
    this->rootServers.push_back("198.41.0.4");
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

    std::string domain;
    while ((domain = Database::getInstance().getNextDomain())!= "")
    {
        bool mustGo = true;
        std::vector<std::string> dnsServers = this->rootServers;
        while (mustGo) {

            for (auto i = dnsServers.begin(); i != dnsServers.end(); ++i) {
                UDPSocket udpSocket;

                DNSPacket dnsPacket;
                dnsPacket.markAsQuestion();
                DNSQuestion dnsQuestion(domain);
                dnsPacket.addQuestion(dnsQuestion);

                SocketAddress address;
                std::string dnsServer = *i;
                int status = address.setAddress(dnsServer);
                address.setPort(53); // DNS port 53
                udpSocket.setAddress(address);

                int size;
                char *data = dnsPacket.getRaw(&size);
                udpSocket.send(data, size);
                free(data);

                char *recive_data = (char *) malloc(MAX_UDP_PACKET_SIZE);
                udpSocket.recive(recive_data, MAX_UDP_PACKET_SIZE);

                DNSPacket recive;
                recive.parseRawBuffer((unsigned char *) recive_data,
                                      MAX_UDP_PACKET_SIZE);
                if (recive.isOk()) {
                    if (recive.getAnswerCount() != 0) {
                        Database::getInstance().updateDomain(domain,
                                                             Domain::FOLLOWED);
                        mustGo = false;
                        break;
                    }
                    if (recive.getAdditionalCount()) {
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
                }
                else {
                    Database::getInstance().updateDomain(domain,
                                                         Domain::NONEXISTENT);
                    mustGo = false;
                    break;
                }
            }
        }
    }
}









