#include "ChatServer.hpp"

void Usage(std::string proc_)
{
    std::cout << proc_ << " login_port message_port" << std::endl;
}
int main(int argc, char *argv[])
{
    if(argc != 3){
        Usage(argv[0]);
        exit(1);
    }
    ChatServer *serp_ = new ChatServer(atoi(argv[1]), atoi(argv[2]));

    serp_->InitServer();
    serp_->Start();
    delete serp_;
	return 0;
}
