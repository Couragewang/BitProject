#include <iostream>
#include "buttonrpc.hpp"

int main()
{
    buttonrpc client;
    client.as_client("127.0.0.1", 5555);
    int a = client.call<int>("inc", 10).val();
    std::cout << "result: " << a << std::endl;
    return 0;
}
