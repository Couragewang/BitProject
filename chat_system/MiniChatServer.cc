#include "MiniChatServer.hpp"
#include "ProtocolUtil.hpp"

static void Usage(std::string proc_)
{
    std::cout << "Usage: " << proc_ << " port" << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        Usage(argv[0]);
        exit(1);
    }
    MiniChatServer *serverp_ = new MiniChatServer(atoi(argv[1]));
    serverp_->InitServer();

    delete serverp_;
    return 0;
}
