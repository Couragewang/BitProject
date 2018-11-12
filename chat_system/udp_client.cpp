#include "udp_client.h"

udp_client::udp_client(std::string _dst_ip, unsigned short int _dst_port):dst_ip(_dst_ip), dst_port(_dst_port)
{
	this->sock = -1;
}

udp_client::~udp_client()
{
	if(this->sock != -1){
		close(this->sock);
	}
}

int udp_client::init()
{
	this->sock = socket(AF_INET, SOCK_DGRAM, 0);
	if( -1 == this->sock ){
		print_log("create sock failed!");
		return 1;
	}
	return 0;
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










