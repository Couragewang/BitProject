#include <iostream>
#include <httplib.h>

using namespace std;

int main()
{
    using namespace httplib;
    Server svr;
    svr.Get("/hi", [](const Request &req, Response &res){
            res.set_content("hello world!", "text/plain");
    });
    svr.listen("localhost", 8080);
    return 0;
}
