#include "GobangServer.hpp"

int inc(int a)
{
    return a + 1;
}

int main()
{
    buttonrpc server;
    server.as_server(5555);
    server.bind("inc", inc);
    server.run();
    return 0;
}
