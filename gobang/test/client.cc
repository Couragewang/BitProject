#include "comm.hpp"

int main()
{
    buttonrpc client;
    client.as_client("127.0.0.1", 5555);
    client.call<test>("fun").val();
    return 0;
}
