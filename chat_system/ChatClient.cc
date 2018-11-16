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

void RunClient(ChatClient *clip)
{
    std::cout << "Login success" << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc != 4){
        Usage(argv[0]);
        exit(1);
    }
    ChatClient *clip_ = new ChatClient( argv[1], atoi(argv[2]), atoi(argv[3]) );
	clip_->InitClient();

    int select = -1;
    while(1){
        Menu();
        std::cin >> select;
        if(select == 1){
            if(clip_->Login()){
                RunClient(clip_);
            }
        }
        else if(select == 2){
            if(clip_->Register()){
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

    delete clip_;
    return 0;
}





