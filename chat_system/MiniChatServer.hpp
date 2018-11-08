#ifndef _MINICHAT_SERVER_HPP_
#define _MINICHAT_SERVER_HPP_

#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "Log.h"

#define EPOLL_NUM 128

class MiniChatServer{
    private:
        int listen_sock;
        int port;
        int epfd;
    public:
        MiniChatServer(const int &sock_, const int &port):sock(sock_),port(port_)
        {}
        void InitServer()
        {
            listen_sock = socket(AF_INET, SOCK_STREAM, 0);
            if(sock < 0){
                LOG(ERROR, "socket error");
                exit(2);
            }
            int opt = 1;
            setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            struct sockaddr_in local;
            bzero(&local, sizeof(local));

            local.sin_family = AF_INET;
            local.sin_port = htons(port);
            local.sin_addr.s_addr = inet_addr(ADDR_ANY);

            if(bind(listen_sock, &local, sizeof(local)) < 0){
                LOG(ERROR, "bind error");
                exit(3);
            }

            if(listen(listen_sock, 5) < 0){
                LOG(ERROR, "listen error");
                exit(4);
            }

            int epfd = epoll_create(EP_NUM);
            if(epfd < 0){
                LOG(ERROR, "epoll_create error");
                exit(5);
            }
        }
        ~MiniChatServer()
        {

        }
};

#endif











