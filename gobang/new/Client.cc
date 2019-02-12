#include "Client.hpp"

static void Usage(std::string proc_)
{
    std::cout <<"Usage: "<< proc_ << " ip port" << std::endl;
}
static void *Count(void *arg)
{
    int time = 0;
    while(time < WAIT_TIME){
        std::cout << "Matching... [ " << time << "/" << WAIT_TIME << " ]S\r";
        fflush(stdout);
        sleep(1);
        time++;
    }
    std::cout << std::endl;
}
static void Menu1()
{
    std::cout << "###########################################" << std::endl;
    std::cout << "##  1. register               2. login   ##" << std::endl;
    std::cout << "##                            3. exit    ##" << std::endl;
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
static void GameBegin(Client *cp, int id, std::string passwd)
{
    int select = -1;
    Menu2();
    std::cin >> select;
    if(select == 1){
        cp->Match();
    }
    else if(select == 2){
        cp->Logout(id, passwd);
    }else{
    }
}
int main(int argc, char *argv[])
{
    if(argc != 3){
        Usage(argv[0]);
        return 1;
    }
    int select = -1;

    std::string name;
    std::string passwd;
    int id;
    std::string ip = argv[1];
    int port = atoi(argv[2]);
    Client *cp = new Client(ip, port);
    while(1){
        Menu1();
        std::cin >> select;
        switch(select){
            case 1:
                {
                    RegisterEnter(name, passwd);
                    if(!cp->Register(name, passwd, id)){
                        exit(6);
                    }
                }
                break;
            case 2:
                {
                    LoginEnter(id, passwd);
                    if(!cp->Login(id, passwd)){
                        exit(7);
                    }
                    GameBegin(cp, id, passwd);
                }
                break;
            case 3:
                exit(0);
            default:
                Usage(argv[0]);
                break;
        }
    }
    return 0;
}
