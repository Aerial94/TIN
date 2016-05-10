#include "DNSPooler.hpp"
#include <chrono>
#include "Database.hpp"
#include "DNSPacket.hpp"
#include "UDPSocket.hpp"

const std::string DNSPooler::rootServers []= {
"192.33.4.12",
"198.41.0.4",
"192.228.79.201",
};

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
    std::string domain;
    while ((domain = Database::getInstance().getNextDomain())!= "")
    {
        UDPSocket udpSocket;
        for (int i = 0; i < sizeof this->rootServers; i++)
        {
            DNSPacket dnsPacket;
            dnsPacket.markAsQuestion();
            DNSQuestion dnsQuestion(domain);
            dnsPacket.addQuestion(dnsQuestion);

            SocketAddress address;
            address.setAddress(this->rootServers[i]);
            address.setPort(53); // DNS port 53
            udpSocket.setAddress(address);

            int size;
            char * data = dnsPacket.getRaw(&size);
            udpSocket.send(data, size);
            free(data);

            char * recive_data = (char *) malloc(MAX_UDP_PACKET_SIZE);
            udpSocket.recive(recive_data, MAX_UDP_PACKET_SIZE);

            DNSPacket recive;
            recive.parseRawBuffer((unsigned char *) recive_data, MAX_UDP_PACKET_SIZE);
        }
    }
}









