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
#include <stdlib.h>
#include <unistd.h>
#include <libltdl/lt_system.h>


void sig_handler(int signo) {
    switch (signo) {
        case SIGINT:
        case SIGTERM:
            Logger::getInstance().logInfo("Exit", "Stopping dns-checker...");
            Logger::getInstance().logInfo("Exit", "Closing log file...");
            Logger::getInstance().close();
            exit(0);
    }
}

int main(int argc, char *argv[]) {
    int opt;
    bool background = true;
    while ((opt = getopt(argc, argv, "fhv")) != -1) {
        switch (opt) {
            case 'f':
                background = false;
                break;
            case 'v':
                fprintf(stdout, "dns-checker v 1.0\n");
                exit(EXIT_SUCCESS);
                break;
            case 'h':
            default:
                fprintf(stderr, "Usage: %s [-fhv]\n", argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }
    try {
        Configuration::getInstance().parse();
    } catch (ConfigurationException &e) {
        std::cout << e.getDescription() << "\n";
        std::cout << "Exiting due to configuration errors\n";
        return -1;
    }
    Logger::getInstance().setLogLevel(
            Configuration::getInstance().getLogLevel());
    Logger::getInstance().logInfo("Main", "Application starting...");
    if (background) {
        Util::deamonize();
    }
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGUSR1, sig_handler);
    DNSPooler dnsPooler(Configuration::getInstance().getDnsPoolerInterval());
    dnsPooler.run();
    HTTPServer server;
    server.listen();
    while (true) {
        pause();
    }
}
