#ifndef _PROTOCOL_UTIL_HPP_
#define _PROTOCOL_UTIL_HPP_

#include <iostream>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>

class Connect{
    private:
        int sock;
    public:
        Connect(int sock_):sock(sock_)
        {}
        ~Connect()
        {}
};

#endif
