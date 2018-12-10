#include <iostream>
#include <string>
#include "buttonrpc.hpp"
#include "GameManager.hpp"

static void Usage(std::string proc_)
{
    std::cout <<"Usage: "<< proc_ << " ip port" << endl;
}

static void Menu()
{
    std::cout << "###########################################" << std::endl;
    std::cout << "##  1. register               2. login   ##" << std::endl;
    std::cout << "##  3. match                  4. logout  ##" << std::endl;
    std::cout << "###########################################" << std::endl;
    std::cout << "Please Enter";
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
void Game(buttonrpc *client, int id)
{
    int x,y;
    volatile bool end = false;
    while(!end){
        client->call<int>("GetBoard", id).val();
        ShowBoard();
        std::cout << "Please Enter Your <x,y>: ";
        std::cin >> x >> y;
        int result = client->call<int>("Game",id, x, y);
        switch(result){
            case -1:
                std::cout << "It is not your turn yet!" << std::endl;
                break;
            case -2:
                std::cout << "Pos be occupied!"
                break;
            case -3:
                std::cout << "Pos is not righe!" << std::endl;
                break;
            case 0:
                {
                    int result = client->call<int>("WhoWin", id);
                    if(result == id){
                        std::cout << "You Win!" << std::endl;
                        end = true;
                    }
                    else if(){
                        std::cout << "You lose!" << std::endl;
                        end = true;
                    }else if(){
                        std::cout << "Tie!" << std::endl;
                        end = true;
                    }else{
                    }
                }
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
        Menu();
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
                    }else{
                        std::cout << "Login Failed!" << std::endl;
                    }
                }
                break;
            case 3:
                {
                    if(client.call<bool>("Match", id).val()){
                        Game(&client, id);
                    }else{
                        std::cout << "Match Failed!" << std::endl;
                    }
                }
                break;
            case 4:
                client.call<bool>("Logout", id).val();
                exit(0);
                break;
            default:
                Usage(argv[0]);
                exit(1);
                break;
        }
    }
    return 0;
}







