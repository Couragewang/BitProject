#ifndef _MINICHAT_SERVER_HPP_
#define _MINICHAT_SERVER_HPP_

#include "Log.hpp"
#include "ProtocolUtil.hpp"

#define EPOLL_NUM 128

class MiniChatServer{
    private:
        int listen_sock;
        int port;
        int epfd;
    public:
        MiniChatServer(const int &port_):listen_sock(-1),port(port_)
        {}
        void InitServer()
        {
            listen_sock = socket(AF_INET, SOCK_STREAM, 0);
            if(listen_sock < 0){
                LOG(ERROR, "socket error");
                exit(2);
            }
            int opt = 1;
            setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            struct sockaddr_in local;
            bzero(&local, sizeof(local));

            local.sin_family = AF_INET;
            local.sin_port = htons(port);
            local.sin_addr.s_addr = inet_addr(INADDR_ANY);

            if(bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
                LOG(ERROR, "bind error");
                exit(3);
            }

            if(listen(listen_sock, 5) < 0){
                LOG(ERROR, "listen error");
                exit(4);
            }

            epfd = epoll_create(EPOLL_NUM);
            if(epfd < 0){
                LOG(ERROR, "epoll_create error");
                exit(5);
            }
            struct epoll_event ev_;
            ev_.events = EPOLLIN;
            ev_.data.ptr = new Connect(listen_sock);
            if(epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &ev_) < 0){
                LOG(ERROR, "epoll add listen sock error");
                exit(6);
            }
        }
        ~MiniChatServer()
        {
            close(listen_sock);
            close(epfd);
        }
};

#endif











