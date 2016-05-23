#include "TCPSocket.hpp"
#include "SocketAddress.hpp"
#include <iostream>
#include "Logger.hpp"
#include "DNSPooler.hpp"
#include "Database.hpp"
#include "HTTPServer.hpp"
#include "Configuration.hpp"
#include "Util.hpp"
#include <signal.h>


#include <iostream>
#include <fstream>


void sig_handler(int signo) {
    if (signo == SIGUSR1) {
        std::ofstream outfile("/tmp/test.txt", std::ios_base::app);
        outfile << "SIGUSR1\n";
        outfile.close();
    }
}

int main(int argc, char *argv[]) {
    try {
        Configuration::getInstance().parse();
    } catch (ConfigurationException &e) {
        std::cout << e.getDescription() << "\n";
        std::cout << "Exiting due to configuration errors\n";
        return -1;
    }
    Logger::getInstance().setLogLevel(Configuration::getInstance().getLogLevel());
    Logger::getInstance().logInfo("Main", "Application starting...");
    //Util::deamonize();
    signal(SIGUSR1, sig_handler);
    DNSPooler dnsPooler(Configuration::getInstance().getDnsPoolerInterval());
    dnsPooler.run();
    HTTPServer server;
	server.listen();
    while (true) {
        pause();
    }
}
