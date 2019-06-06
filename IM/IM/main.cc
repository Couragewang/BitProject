#include "IM_Server.hpp"

using namespace std;

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " port" << std::endl;
    std::cout << "Notice: "<<"\n\tDefault Port: 8080" << "\n\tPort Range: [1024-9999]"<< std::endl;
}
static bool IsPortOk(const char* _port, std::string &port_out)
{
    int p = atoi(_port);
    if(p >= 1024 && p <= 9999){
        port_out = _port;
        return true;
    }
    return false;
}
int main(int argc, char *argv[])
{
    int ret = 0;
    std::string port = "8080";
    if( (argc == 2 && IsPortOk(argv[1], port)) || argc == 1 ){
        IM_Server isp(port);
        isp.InitServer();
        isp.Run();
    }
    else{
        Usage(argv[0]);
        ret = 1;
    }
    return ret;
}





