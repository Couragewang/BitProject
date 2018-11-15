#include "ChatClient.hpp"

static void Usage(std::string proc_)
{
    std::cout << proc_ << " server_ip port login_port" << std::endl;
}

static void Menu()
{
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "| 1. Login                 2. Register |" << std::endl;
    std::cout << "|                          0. Exit     |" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Please Select# ";
}
int main(int argc, char argv[])
{
    if(argc != 4){
        Usage(argv[0]);
        exit(1);
    }
    ChatClient *clip = new ChatClient( argv[1], atoi(argv[2]), atoi(argv[3]) );

    int select = -1;
    while(1){
        Menu();
        cin >> select;
        if(select == 1){
            if(clip->Login()){
                //todo
            }
        }
        else if(select == 2){
            if(clip->Register()){
                std::cout << "Register Success! Please Login!" << std::endl;
            }
            else{
                std::cout << "Register Failed! Please Try Again!" << std::endl;
            }
        }
        else{
            break;
        }
    }

    delete clip;
    return 0;
}





