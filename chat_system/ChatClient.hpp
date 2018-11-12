#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <string>
#include <map>
#include <arpa/inet.h>
#include <vector>
#include <strstream>
#include "udp_log.h"
#include "udp_data.h"

class udp_client{
	public:
		udp_client(std::string _dst_ip="127.0.0.1", unsigned short int _dst_port=8888);
		~udp_client();
		int init();
		ssize_t send_msg(const std::string &_msg);
		ssize_t recv_msg(std::string &_msg);
		void add_friend(const std::string &_key);
		void del_friend(const std::string &_key);
		std::vector<std::string> &get_friends_list(){ return this->friends_list;}

		int set_ip(const std::string &_ip){this->dst_ip = _ip;}
		int set_port(unsigned short int _port){this->dst_port = _port;}
	private:
		bool is_friend_exist(const std::string &_key);
	private:
		int sock;
		std::string dst_ip;
		unsigned short int dst_port;
		std::vector<std::string> friends_list;
};

#endif

