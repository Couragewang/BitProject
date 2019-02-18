#include "GameServer.hpp"

int main()
{
    GameServer *sp = new GameServer();
    sp->InitServer();
    sp->Start();
    return 0;
}
