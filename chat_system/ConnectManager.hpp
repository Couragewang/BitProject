#ifndef __CONNECT_MANAGE_HPP__
#define __CONNECT_MANAGE_HPP__

#include <iostream>
#include "ProtocolUtil.hpp"

class Connect{
    private:
        int sock;
        char message[256];
    public:
        Connect(int sock_):sock(sock_)
        {}
        int GetSock()
        {
            return sock;
        }
        ~Connect()
        {}
};

class ConnectManager{

};

#endif
