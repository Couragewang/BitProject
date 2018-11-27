#include "ChatClient.hpp"
#include "ChatWindow.hpp"

static void Usage(std::string proc_)
{
    std::cout << proc_ << " server_ip" << std::endl;
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
    ChatWindow *winp = new ChatWindow();
    winp->Start(clip);
    delete winp;
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        Usage(argv[0]);
        exit(1);
    }
    ChatClient *clip_ = new ChatClient( argv[1] );
	clip_->InitClient();
    int select = -1;
    while(1){
        Menu();
        std::cin >> select;
        if(select == 1){
            if(clip_->Login()){
                std::cout << "Login success!" << std::endl;
                RunClient(clip_);
            }
            else{
                std::cout << "Login Failed, Please Check Your ID or Passwd!" <<std::endl;
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





