#include "TCPSocket.hpp"
#include "SocketAddress.hpp"
#include <iostream>

int main(int argc, char * argv[])
{
    TCPSocket s;
    s.setTimeout(0, 500);
    s.setReuseAddress();
    s.serveForever(1337);
    std::string data;
    while (true)
    {
        data.clear();
        TCPSocket client = s.accept();
        client >> data;
        std::cout << data;
        std::cout << "\n";
        client << data;
    }
}
