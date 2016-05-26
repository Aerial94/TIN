#include "Util.hpp"
#include "Logger.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <cstdio>

void Util::deamonize() {
    std::puts("deamonizing...");
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
        FILE * f = fopen("dns-checker.pid", "w");
        fprintf(f, "%d\n", pid);
        fclose(f);
        exit(EXIT_SUCCESS);
    }
    umask(0);

    pid_t sid = setsid();
    if (sid < 0) {
        Logger::getInstance().logWarning("deamonize", "Cant beacome session leader");
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        Logger::getInstance().logWarning("deamonize", "Cant change working directory to /");
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

