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
class Register_Request{
    public:
        std::string nick_name;
        std::string passwd;

        void Serialize(std::string &message_)
        {
        	Json::Value value_;
            value_["nick_name"]        = nick_name;
        	value_["passwd"]       = passwd;
            Util::ValueToMessage(value_, message_);
        }
        void Deserialize(std::string &message_)
        {
        	Json::Value value_;
            Util::MessageToValue(message_, value_);

        	nick_name       = value_["nick_name"].asString();
        	passwd          = value_["passwd"].asString();
        }
};
//登录协议&&退出协议
class LL_Request{ //login&&logout
    public:
        int player_id;
        std::string passwd;

        void Serialize(std::string &message_)
        {
        	Json::Value value_;
            value_["player_id"]        = player_id;
        	value_["passwd"]       = passwd;
            Util::ValueToMessage(value_, message_);
        }
        void Deserialize(std::string &message_)
        {
        	Json::Value value_;
            Util::MessageToValue(message_, value_);

        	player_id       = value_["player_id"].asInt();
        	passwd          = value_["passwd"].asString();
        }
};

//匹配响应
class Match_Resquest{

};
//请求相应
class Response{
    public:
        int player_id;
        int status;

        void Serialize(std::string &message_)
        {
        	Json::Value value_;
            value_["player_id"]        = player_id;
        	value_["status"]       = status;
            Util::ValueToMessage(value_, message_);
        }
        void Deserialize(std::string &message_)
        {
        	Json::Value value_;
            Util::MessageToValue(message_, value_);

        	player_id       = value_["player_id"].asInt();
        	status          = value_["status"].asInt();
        }
};

#endif
