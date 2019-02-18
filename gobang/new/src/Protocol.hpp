#ifndef _PROTOCOL_HPP_
#define _PROTOCOL_HPP_

#include <iostream>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Serializer.hpp"

#define NUM 128

class SocketApi{
    public:
        static int Socket()
        {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if(sock < 0){
                std::cerr << "socket error" << std::endl;
                exit(2);
            }
            return sock;
        }
        static void Bind(int sock, int port)
        {
            struct sockaddr_in local;
            bzero(&local, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(port);
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0){
                std::cerr << "bind error" << std::endl;
                exit(3);
            }
        }
        static void Listen(int sock)
        {
            if(listen(sock, 5) < 0){
                std::cerr << "listen error" << std::endl;
                exit(4);
            }
        }
        static int Accept(int listen_sock)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
            if(sock < 0){
                std::cerr << "listen error" << std::endl;
            }
            return sock;
        }
        static void Connect(int sock, std::string ip, int port)
        {
            struct sockaddr_in peer;
            bzero(&peer, sizeof(peer));
            peer.sin_family = AF_INET;
            peer.sin_port = htons(port);
            peer.sin_addr.s_addr = inet_addr(ip.c_str());
            if(connect(sock, (struct sockaddr*)&peer, sizeof(peer)) < 0){
                std::cerr << "connect error" << std::endl;
                exit(5);
            }
        }
        static int Recv(int sock, std::string &out)
        {
            char buf[1024];
            ssize_t s = recv(sock, buf, sizeof(buf), 0);
            if(s > 0){
                buf[s] = 0;
                std::cout << buf << std::endl;
                out = buf;
            }
            return s;
        }
        static int Send(int sock, std::string &in)
        {
            std::cout << in << std::endl;
            return send(sock, in.c_str(), in.size(), 0);
        }

};

//注册协议
struct Register_Request{
    char nick_name[NUM];
    char passwd[NUM];
};
//登录协议
struct Login_Request{ //login&&logout
    int player_id;
    char passwd[NUM];
};
//退出协议
struct Logout_Request{
    int player_id;
};
//匹配协议
class Match_Resquest{
    int player_id;
};
//响应
struct Response{
    int status;
};
#endif






