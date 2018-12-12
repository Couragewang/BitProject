#include "GameManager.hpp"
#include "PlayerManager.hpp"

#define PORT 8888

int main()
{
    PlayerManager pm;
    buttonrpc server;
    server.as_server(8888);

    server.bind("Register", &PlayerManager::Register, &pm); //OK
    server.bind("Login", &PlayerManager::Login, &pm); //OK
    server.bind("PlayerChessColor", &PlayerManager::PlayerChessColor, &pm);
    server.bind("Logout", &PlayerManager::Logout, &pm);//OK
    server.bind("Match", &PlayerManager::Match, &pm);
    server.bind("Game", &PlayerManager::Game, &pm);
    server.bind("GetRoom", &PlayerManager::GetRoom, &pm);
    server.bind("GameEnd", &PlayerManager::GameEnd, &pm);

    GameManager gm(&pm);
    gm.StartMatchThread();

    server.run();
    return 0;
}
