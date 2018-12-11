#include "PlayerManager.hpp"
#include "GameManager.hpp"

#define PORT 8888

int main()
{
    buttonrpc server;
    server.as_server(8888);

    PlayerManager pm;
    GameManager gm(&pm);

    server.bind("Register", &pm::Register, &pm);
    server.bind("PlayerChessColor", &pm::PlayerChessColor, &pm);
    server.bind("Login", &pm::Login, &pm);
    server.bind("Logout", &pm::Logout, &pm);
    server.bind("Match", &pm::Match, &pm);
    server.bind("Game", &pm::Game, &pm);
    server.bind("GetRoom", &pm::GetRoom, &pm);
    server.bind("GameEnd", &pm::GameEnd, &pm);

    gm.StartMatchThread();

    server.run();
    return 0;
}
