#include "IM_Server.hpp"

using namespace std;

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " port [default:8080]" << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        Usage(argv[0]);
        return 1;
    }
    IM_Server isp(argv[1]);
    isp.InitServer();
    isp.Run();
}





