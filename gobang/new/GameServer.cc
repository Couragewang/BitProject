#include "Hall.hpp"

int main()
{
    Server *sp = new Server();
    sp->InitServer();
    sp->Start();
    return 0;
}
