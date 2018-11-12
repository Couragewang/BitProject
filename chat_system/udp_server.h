#ifndef _UDP_SERVER_H_
#define _UDP_SERVER_H_

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <string>
#include <map>
#include <arpa/inet.h>
#include "udp_log.h"
#include "udp_data.h"
#include "data_pool.h"

class udp_server{
	public:
		udp_server(std::string _host_ip="127.0.0.1",\
				unsigned short int _host_port=8888);
		~udp_server();
		int init();
		int run();
		ssize_t recv_msg();
		int broadcast_msg();
	private:

	    ssize_t send_msg(const struct sockaddr_in &_cli,\
				const socklen_t &_len, const std::string &_msg);
	    int analysis_client(const struct sockaddr_in &cli, \
				const socklen_t &len,std::string &);

	private:
		int sock;
		std::string host_ip;
		unsigned short int host_port;
		std::map<std::string, struct sockaddr_in> online_user;
		data_pool msg_pool;
};

#endif

