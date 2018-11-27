#include "ChatServer.hpp"

void Usage(std::string proc_)
{
    std::cout << "Usage: " << proc_ << std::endl;
}
int main(int argc, char *argv[])
{
    if(argc != 1){
        Usage(argv[0]);
        exit(1);
    }
    ChatServer *serp_ = new ChatServer();

    serp_->InitServer();
    serp_->Start();
    delete serp_;
	return 0;
}
