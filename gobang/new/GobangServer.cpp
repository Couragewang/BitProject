#include <iostream>
#include <pthread.h>
#include "rpc_server.h"
#include "server/Hall.hpp"

using namespace rest_rpc;
using namespace rpc_service;

#define PORT 8888
#define SIZE 2

Hall h;
PlayerManager &pm = h.pm;
RoomManager &rm   = h.rm;
MatchManager &mm  = h.mm;

int Register(connection* conn, const std::string &name, const std::string &passwd)
{
    return pm.Register(name, passwd);
}
bool Login(connection* conn, const int &id, const std::string &passwd)
{
    return pm.Login(id, passwd);
}
bool Logout(connection *conn, const int &id, const std::string &passwd)
{
    return pm.Logout(id, passwd);
}
int Match(connection *conn, const int &id)
{
    return h.PlayerMatch(id);
}
void Move(connection *conn, const int &id, const int &room_id, const int &x, const int &y)
{
    rm.Move(id, room_id, x, y);
}
std::string GetBoard(connection *conn, const int &room_id)
{
    return rm.GetRoomBoard(room_id);
}
bool WhoPlay(connection *conn, const int &id, const int &room_id)
{
    return rm.WhoPlay(id, room_id);
}
int Judge(connection *conn, const int &room_id)
{
    return rm.Judge(room_id);
}
static void *ThreadRoutine(void *arg)
{
    Hall *hp = (Hall*)arg;
    pthread_detach(pthread_self());
    hp->MatchService();
}
int main()
{
	rpc_server server(PORT, SIZE);
    server.register_handler("Register", Register);
	server.register_handler("Login", Login);
	server.register_handler("Logout", Logout);
	server.register_handler("Match", Match);
    server.register_handler("Move", Move);
    server.register_handler("GetBoard", GetBoard);
    server.register_handler("WhoPlay", WhoPlay);
    server.register_handler("Judge", Judge);
    pthread_t tid;
    pthread_create(&tid, NULL, ThreadRoutine, &h);

	server.run();
}
