#include "DNSPooler.hpp"
#include "Database.hpp"
#include "HTTPServer.hpp"
#include "Logger.hpp"
#include "TCPSocket.hpp"
#include "Util.hpp"
#include <atomic>
#include <signal.h>
#include <stdlib.h>

std::atomic_bool stopThreads;
static void hdl(int sig, siginfo_t *siginfo, void *context) {
    switch (sig) {
    case SIGINT:
    case SIGTERM:
        stopThreads = true;
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

    sigset_t signal_set;
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGINT);
    sigaddset(&signal_set, SIGTERM);
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_sigaction = &hdl;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    DNSPooler dnsPooler(Configuration::getInstance().getDnsPoolerInterval());
    dnsPooler.run();
    HTTPServer server;
    server.run();
    stopThreads = false;
    while (true) {
        if (stopThreads) {
            sigprocmask(SIG_BLOCK, &signal_set, NULL);
            Logger::getInstance().logInfo("Main", "Stopping dns-checker...");
            dnsPooler.stop();
            server.stop();
            Logger::getInstance().logInfo("Main", "Done stopping dns-checker");
            Logger::getInstance().stop();
            sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
            break;
        }
        pause();
    }
}
