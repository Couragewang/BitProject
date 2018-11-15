#ifndef __CHAT_CLIENT_HPP__
#define __CHAT_CLIENT_HPP__

#include <iostream>
#include <strings.h>
#include <string.h>
#include <vector>
#include <strstream>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "ProtocolUtil.hpp"
#include "Log.hpp"

class ChatClient{
	private:
		int sock;
        int port; //message udp port
        int login_sock; //login and register tcp port
        int login_port;
		std::string server_ip;

	public:
		ChatClient(std::string server_ip_="127.0.0.1", int port_= 8080, int login_port_ = 8081)
            :server_ip(server_ip_), port(port_), login_port(login_port_), sock(-1), login_sock(-1)
        {}
		~ChatClient()
        {
            if(sock > 0){
                close(sock);
            }
            if(login_sock > 0){
                close(login_sock);
            }
        }
		void InitClient()
        {
	        sock = socket(AF_INET, SOCK_DGRAM, 0);
	        if( -1 == sock ){
	        	Log(ERROR, "create sock failed!");
                exit(2);
	        }
            login_sock = socket(AF_INET, SOCK_STREAM, 0);
            if( -1 == login_sock ){
                Log(ERROR, "create login sock failed!");
                exit(3);
            }
        }
        bool ConnectServer()
        {
            struct sockaddr_in peer_;
            bzero(&peer_, sizeof(peer_));
            peer_.sin_family = AF_INET;
            peer_.sin_port = htons(login_port);
            peer_.sin_addr.s_addr = inet_addr(server_ip.c_str());

            if(connect(login_sock, (struct sockaddr*)&peer_, sizeof(peer_)) < 0){
                Log(ERROR, "connect error!");
                return false;
            }
            return true;
        }
        bool Login()
        {
            if(!ConnectServer()){
                return false;
            }
            char type = LOGIN;
            send(login_sock, &type, 1, 0);

            std::string passwd_;
            struct LoginInfo lg_;
            std::cout << "UserID:";
            std::cin >> lg_.id;
            Util::EnterPasswd("Password:",passwd_);
            strcpy(lg_.passwd, passwd_.c_str());
            send(login_sock, &lg_, sizeof(lg_), 0);

            bool ret;
            struct Reply rp_;
            recv(login_sock, &rp_, sizeof(rp_), 0);
            if(rp_.status == 0){
                ret = true;
            }else{
                ret = false;
            };

            close(login_sock);
            return ret;
        }
        bool Resister()
        {
            if( !ConnectServer() ){
                return false;
            }
            char type_ = REGISTER;
            send(login_sock, &type_, sizeof(type_), 0);

            struct RegisterInfo reg_;
            std::string passwd_;
            std::cout << "Please Enter Your Nick Name: ";
            std::cin >> reg_.nick_name;
            std::cout << "Please Enter Your School: ";
            std::cin >> reg_school;
            while(1){
                Util::EnterPasswd("Please Enter Password: ",reg_.passwd);
                Util::EnterPasswd("Please Enter Password Again: ",passwd_);
                if(reg_.passwd == passwd_){
                    break;
                }else{
                    std::cout << "Password Inconsistent! Try Again!" << std::endl;
                }
            }

            send(login_sock, &reg_, sizeof(reg_), 0);

            bool ret;
            struct Reply rp_ = {-1, -1};
            recv(login_sock, &rp_, sizeof(rp_), 0);
            if(rp_.status == 0){
                std::cout << "Your Login Id Is : " << rp_.id << " Please Remember!" << std::endl;
                ret = true;
            }else{
                std::cout << "Register Error!" << std::endl;
                ret = false;
            }

            close(login_sock);
            return ret;
        }
		ssize_t SendMessage(const std::string &message_)
        {
	        struct sockaddr_in server_;
	        socklen_t len_ = sizeof(struct sockaddr_in);
	        bzero(&server_, len_);

	        server_.sin_family = AF_INET;
	        server_.sin_port   = htons(port);
	        server_.sin_addr.s_addr = inet_addr(server_ip.c_str());
	        ssize_t size_ = sendto(sock, message_.c_str(), message_.size(),\
	        		0, (struct sockaddr*)&server_, len_);
	        return size_;
        }
		ssize_t RecvMessage(std::string &message_)
        {
	        char buf_[MESSAGE_SIZE];
	        memset(buf_, 0, sizeof(buf_));

	        struct sockaddr_in dst_;
	        socklen_t len_ = sizeof(dst_);
	        bzero(&dst_, len_);

	        ssize_t size_ = recvfrom(sock, buf_, sizeof(buf_), 0, (struct sockaddr*)&dst_, &len_);
	        if(size_ > 0){
	        	message_ = buf_;
	        }
	        return size_;
        }
};

#endif
