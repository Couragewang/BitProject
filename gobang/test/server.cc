#include "comm.hpp"

int main()
{
    buttonrpc server;
    server.as_server(5555);
    test t;
    server.bind("fun", &test::show, &t);
    server.run();
    return 0;
}












