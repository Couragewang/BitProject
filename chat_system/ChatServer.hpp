#ifndef _MINICHAT_SERVER_HPP_
#define _MINICHAT_SERVER_HPP_

#include "ProtocolUtil.hpp"
#include "ConnectManage.hpp"
#include "UserManage.hpp"
#include "Log.hpp"

#define EPOLL_NUM 128

#define EV_READ_READY(ev) ((ev) & EPOLLIN)
#define EV_WRITE_READY(ev) ((ev) & EPOLLOUT)

class MiniChatServer{
    private:
        int listen_sock;
        int port;
        int epfd;
    private:
        void AcceptNewConnect()
        {
            struct sockaddr_in peer_;
            socklen_t len_ = sizeof(peer_);
            int sock_ = accept(listen_sock, &peer_, &len_);
            if(sock_ < 0){
                LOG(ERROR, "accept new connect error");
                return;
            }

        }
        void ProcessRequest(struct epoll_event revs_[], int rnums_)
        {
            int i_ = 0;
            for(; i_ < rnums_; i_++){
                Connect *conn_ = (Connect *)(revs_[i].data.ptr);
                int sock_ = conn_->GetSock();
                uint32_t ev_ = revs_[i].events;
                if(sock_ == listen_sock && EV_READ_READY(ev_)){
                    AcceptNewConnect();
                    continue;
                }
            }
        }
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
            local.sin_addr.s_addr = htonl(INADDR_ANY);

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

        void Start()
        {
            struct epoll_event revs_[EPOLL_NUM];
            int revs_num_ = EPOLL_NUM;
            for( ; ; ){
                int timeout_ = 1000;
                int ready_num_ = 0;
                switch((ready_num_ = epoll_wait(epfd, revs_, revs_num_, timeout_))){
                    case 0: //timeout
                        std::cout << "time out..." << std::endl;
                        break;
                    case -1:
                        std::cout << "epoll error..." << std::endl;
                        break;
                    default:
                        ProcessRequest(revs_, ready_num_);
                        break;
                }
            }
        }

        ~MiniChatServer()
        {
            epoll_ctl(epfd, EPOLL_CTL_DEL, listen_sock, NULL);
            close(listen_sock);
            close(epfd);
        }
};

#endif











