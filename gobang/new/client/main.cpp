#include <iostream>
#include <string>
#include "Util.hpp"
#include "Gobang.hpp"

#define REGISTER 1
#define LOGIN 2
#define MATCH 3
#define LOGOUT 4

int main(int argc, char *argv[])
{
    if(argc != 3){
        Util::Usage(argv[0]);
        exit(1);
    }
    std::string ip = argv[1];
    std::string port = argv[2];
    Gobang *gbp = new Gobang(ip, port);

    std::string name;
    std::string passwd;
    int select1 = 1;
    int id = -1;

    for( ; ; ){
        Util::Menu1();
        std::cin >> select1;

        switch(select1){
            case REGISTER:
                {
                    Util::RegisterEnter(name,passwd);
                    id = gbp->Register(name, passwd);
                    if(id < 0){
                        std::cout << "Register Error!" << std::endl;
                        exit(2);
                    }else{
                        std::cout << "Register Done, Your Login Id Is : " << id << std::endl;
                    }
                }
                break;
            case LOGIN:
                {
                    Util::LoginEnter(id, passwd);
                    if(gbp->Login(id, passwd)){
                        std::cout << "Login Success!" << std::endl;
                        int select2;
                        Util::Menu2();
                        std::cin >> select2;
                        switch(select2){
                            case MATCH:
                                {
                                    int room;
                                    if(gbp->Match(id, passwd, room)){
                                        gbp->Game(id, room);
                                    }
                                }
                                break;
                            case LOGOUT:
                                {
                                    if(gbp->Logout(id, passwd)){
                                        std::cout << "Logout Success" << std::endl;
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                    }else{
                        std::cout << "Login Failed!" << std::endl;
                    }
                }
                break;
            default:
                {
                    exit(0);
                    //TODO
                }
                break;
        }
    }

    delete gbp;
	return 0;
}

