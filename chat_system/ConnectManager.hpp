#ifndef __CONNECT_MANAGE_HPP__
#define __CONNECT_MANAGE_HPP__

#include <iostream>

class Connect{
    private:
        int sock;
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
