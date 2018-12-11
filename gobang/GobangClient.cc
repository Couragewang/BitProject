#include <iostream>
#include <string>
#include "buttonrpc.hpp"
#include "Comm.hpp"

static void Usage(std::string proc_)
{
    std::cout <<"Usage: "<< proc_ << " ip port" << endl;
}
static void Menu1()
{
    std::cout << "###########################################" << std::endl;
    std::cout << "##  1. register               2. login   ##" << std::endl;
    std::cout << "###########################################" << std::endl;
    std::cout << "Please Select# ";
}
static void Menu2()
{
    std::cout << "###########################################" << std::endl;
    std::cout << "## 1. Match                   2. logout  ##" << std::endl;
    std::cout << "###########################################" << std::endl;
    std::cout << "Please Select# ";
}
void RegisterEnter(std::string &name, std::string &passwd)
{
    std::string p1,p2;
    std::cout << "Please Enter Nick Name: ";
    std::cin >> name;
    std::cout << "Please Enter Passwd: ";
    std::cin >> p1;
    std::cout << "Please Enter Passwd Again: ";
    std::cin >> p2;
    if(p1 != p2){
        std::cout << "Enter Error" << std::endl;
        exit(2);
    }
    passwd = p1;
}
static void LoginEnter(int &id, std::string &passwd)
{
    std::cout << "Please Enter Login Id: ";
    std::cin >> id;
    std::cout << "Please Enter Passwd: ";
    std::cin >> passwd;
}
void Play(buttonrpc &client, int &id, char &chess_color)
{
    int x,y;
    Room room_;
    volatile is_quit = false;
    while(!is_quit){
        room_.ShowBoard();
        std::cout << "Please Enter Your Pos<x,y>: ";
        std::cin >> x >> y;

        switch(client->call<int>("Game",id, x, y)){
            case -1:
                std::cout << "It is not your turn yet!" << std::endl;
                break;
            case -2:
                std::cout << "Pos be occupied!" << std::endl;
                break;
            case -3:
                std::cout << "Pos is not right!" << std::endl;
                break;
            case 0:
                {
                    room_ = client.call<Room>("GetRoom", id);
                    char result = room_.judge();
                    if(result != 'N'){
                        if(result == 'E'){
                            std::cout << "Tie Over!" << std::endl;
                        }
                        if(result == chess_color){
                            std::cout << "You Win!" << std::endl;
                        }
                        else{
                            std::cout << "You Lose!" << std::endl;
                        }
                        client.call<void>("GameEnd", id);
                        return;
                    }
                }
                break;
            default:
                break;
        }

    }
}
void Game(buttonrpc &client, int &id)
{
    int select;
    int x,y;
    volatile bool end = false;
    while(!end){
        Menu2();
        std::cout >> select;
        switch(select){
            case 1:
                {
                    if(client.call<bool>("Match", id).val()){
                        char chess_color = client.call<char>("PlayerChessColor", id);
                        Play(client, id, chess_color);
                    }else{
                        std::cout << "Match Failed!" << std::endl;
                    }
                }
                break;
            case 2:
                client.call<bool>("Logout", id).val();
                break;
            default:
                break;
        }
    }
}
int main(int argc, char *argv[])
{
    if(argc != 3){
        Usage(argv[0]);
        return 1;
    }
    int select = -1;
    buttonrpc client;
    client.as_client(argv[1], atoi(argv[2]));
    std::string name;
    std::string passwd;
    int id;
    while(1){
        Menu1();
        std::cin >> select;
        switch(select){
            case 1:
                RegisterEnter(name, passwd);
                id = client.call<int>("Register", name, passwd).val();
                std::cout << "register success, please login!" << std::endl;
                break;
            case 2:
                {
                    LoginEnter(id, passwd);
                    if(client.call<bool>("Login", id, passwd).val()){
                        std::cout << "Login Success!" << std::endl;
                        Game(client, id);
                    }else{
                        std::cout << "Login Failed!" << std::endl;
                    }
                }
                break;
            default:
                Usage(argv[0]);
                exit(1);
                break;
        }
    }
    return 0;
}

