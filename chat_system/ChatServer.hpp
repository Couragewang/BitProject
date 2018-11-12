#ifndef __CHAT_SERVER_HPP__
#define __CHAT_SERVER_HPP__

#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ProtocolUtil.hpp"
#include "MessagePool.hpp"
#include "Log.hpp"

class ChatServer{
    private:
        int sock;
        int port;
        MessagePool pool;

    private:
        void RecvMessage(std::string &message_)
        {
	        char msg_[MESSAGE_SIZE];

	        struct sockaddr_in client_;
	        socklen_t len_ = sizeof(client_);
	        bzero(&cli_, len_);

	        ssize_t size_ = recvfrom(sock,msg_, sizeof(msg_), 0, (struct sockaddr *)&client_, &len_);
	        if( -1 == size_){
	        	Log(ERROR, "recv data from client failed!");
	        }
            else{
	        	Log(INFO, "recv data from client success!");
	        	message_ = msg_;
	        }
        }
        void SendMessage(const std::string &message_, struct sockaddr_in &client_, socklen_t &len_)
        {
	        ssize_t size_ = sendto(sock, message_.c_str(), message_.size(), 0, (struct sockaddr*)&client_, len_);
	        if( size_ < 0 ){
	        	Log(ERROR, "send data to client failed!");
	        }else{
	        	Log(INFO, "send data to client success!");
	        }
        }
    public:
        ChatServer(int port_):sock(-1), port(port_)
        {}
        void InitServer()
        {
	        sock = socket(AF_INET, SOCK_DGRAM, 0);
	        if(this->sock < 0){
	        	Log(ERROR, "create sock error!");
	        	exit(2);
	        }
	        LOG(INFO, "create sock success!");

	        struct sockaddr_in local_;
	        bzero(&local_, sizeof(local_));

	        local_.sin_family = AF_INET;
	        local_.sin_port   = htons(port);
	        local_.sin_addr.s_addr = htonl(INADDR_ANY);
	        socklen_t len_ = sizeof(struct sockaddr_in);

	        if( -1 == bind(sock, (struct sockaddr*)&local_, len_)){
	        	Log(ERROR, "bind sock error!");
                exit(3);
	        }
	        Log(INFO, "bind sock success!");
	        Log(INFO, "initialize server success!");
        }
        void Start()
        {
            pthread_
        }
        ~ChatServer()
        {
            close(sock);
        }
};

#endif












#include "udp_server.h"
#include "udp_data.h"
udp_server::udp_server(std::string _host_ip,unsigned short int _host_port):host_ip(_host_ip), host_port(_host_port)
{
	this->sock = -1;
}

udp_server::~udp_server()
{
	if( -1 != this->sock ){
		close(this->sock);
	}
}
/// \this is server init function
/// \return 0 : success
/// \return None 0 : failed 
int udp_server::init()
{
}

int udp_server::run()
{
	return 0;
}

ssize_t udp_server::recv_msg()
{
}

int udp_server::analysis_client(const struct sockaddr_in &cli, const socklen_t &len, std::string &_msg)
{
	std::string _ip_key = inet_ntoa(cli.sin_addr);
	std::map<std::string, struct sockaddr_in>::iterator _iter = online_user.find(_ip_key);

	if( _iter == online_user.end()){
		online_user.insert(std::pair<std::string, struct sockaddr_in>(_ip_key, cli));
		//printf log
		std::string _log = "get a new cli, ";
		_log += _ip_key;
		print_log(_log);
	}else{
		udp_data data;
		data.str_to_val(_msg);
		std::string _type = data.get_type();
		std::string _cmd  = data.get_cmd();
		if(_type == "CMD" && _cmd == "QUIT"){
			online_user.erase(_iter);
		}
	}
	return 0;
}

int udp_server::broadcast_msg()
{
	std::string _msg_out;
	msg_pool.get_msg(_msg_out);

	std::map<std::string, struct sockaddr_in>::iterator _iter = online_user.begin();
	for(; _iter != online_user.end(); _iter++ ){
		socklen_t len = sizeof(_iter->second);
		ssize_t ret = send_msg(_iter->second, len, _msg_out);//send error
	}
}

ssize_t udp_server::send_msg(const struct sockaddr_in &_cli,const socklen_t &_len, const std::string &_msg)
{
	return _size;
}
