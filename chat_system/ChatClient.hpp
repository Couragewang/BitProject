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

            struct LoginInfo lg_;
            std::cout << "UserID:";
            std::cin >> lg_.id;
            std::cout << "Password:";
            std::cin >> lg_.passwd;
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
                std::cout << "Please Enter Password: ";
                std::cin >> reg_.passwd;
                std::cout << "Please Enter Password Again: ";
                std::cin >> passwd_;
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
		ssize_t send_msg(const std::string &_msg);
		ssize_t recv_msg(std::string &_msg);
		void add_friend(const std::string &_key);
		void del_friend(const std::string &_key);
		std::vector<std::string> &get_friends_list(){ return this->friends_list;}

		int set_ip(const std::string &_ip){this->dst_ip = _ip;}
		int set_port(unsigned short int _port){this->dst_port = _port;}
	private:
		bool is_friend_exist(const std::string &_key);
};

#endif
int udp_client::init()
{
}

ssize_t udp_client::send_msg(const std::string &_msg)
{
	struct sockaddr_in dst;
	socklen_t len = sizeof(struct sockaddr_in);
	bzero(&dst, len);

	dst.sin_family = AF_INET;
	dst.sin_port   = htons(this->dst_port);
	dst.sin_addr.s_addr = inet_addr(this->dst_ip.c_str());
	ssize_t _size = sendto(this->sock, _msg.c_str(), _msg.size(),\
			0, (struct sockaddr*)&dst, len);

	if( -1 == _size ){
		print_log("sendto data error!");
	}
	return _size;
}

ssize_t udp_client::recv_msg(std::string &_msg)
{
	char buf[BUF_SIZE];
	memset(buf, 0, sizeof(buf));

	struct sockaddr_in dst;
	socklen_t len = sizeof(dst);
	bzero(&dst, len);

	ssize_t _size = recvfrom(this->sock, buf, sizeof(buf), 0, \
			(struct sockaddr*)&dst, &len);
	if( -1 == _size ){
		print_log("get msg failed!");
	}else{
		_msg = buf;
	}
	return _size;
}

bool udp_client::is_friend_exist(const std::string &_key)
{
	std::vector<std::string>::iterator _iter=friends_list.begin();
	for( ; _iter != friends_list.end(); _iter++){
		if( *_iter == _key ){
			return true;
		}
	}
	return false;
}

void udp_client::add_friend(const std::string &_key)
{
	if(is_friend_exist(_key)){
		//Do Nothing
	}else{
		friends_list.push_back(_key);
	}
}

void udp_client::del_friend(const std::string &_key)
{
	if(is_friend_exist(_key)){
		std::vector<std::string>::iterator _iter = friends_list.begin();
		for( ; _iter != friends_list.end(); ){
			if( *_iter == _key ){
				_iter = friends_list.erase(_iter);
				return;
			}else{
				_iter++;
			}
		}
	}else{
		//Do Nothing
	}
}

#endif
