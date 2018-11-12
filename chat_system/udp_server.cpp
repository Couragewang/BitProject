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
	this->sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(this->sock < 0){
		print_log("create sock error!");
		return 1;
	}
	print_log("create sock success!");
	struct sockaddr_in local;
	bzero(&local, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_port   = htons(this->host_port);
	local.sin_addr.s_addr = inet_addr(this->host_ip.c_str());
	socklen_t len = sizeof(struct sockaddr_in);
	if( -1 == bind(this->sock, (struct sockaddr*)&local, len)){
		print_log("bind sock error!");
		return 2;
	}
	print_log("bind sock success!");
	return 0;
}

int udp_server::run()
{
	return 0;
}

ssize_t udp_server::recv_msg()
{
	char buf[BUF_SIZE];
	memset(buf, 0, sizeof(buf));

	struct sockaddr_in cli;
	socklen_t len = sizeof(cli);
	bzero(&cli, len);
	ssize_t _size = recvfrom(this->sock,buf, sizeof(buf), 0, (struct sockaddr *)&cli, &len);
	if( -1 == _size){
		print_log("recv data from cli failed!");
	}else{
		//printf log
		std::string _log = "recv data from cli success!";
		_log += buf;
		print_log(_log);

		std::string _msg;
		_msg = buf;
		msg_pool.put_msg(_msg);
		analysis_client(cli, len, _msg);
	}
	return _size;
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
	ssize_t _size = sendto(this->sock, _msg.c_str(), _msg.size(), 0, (struct sockaddr*)&_cli, _len);
	if( _size == -1 ){
		print_log("send data to cli failed!");
	}else{
		print_log("send data to cli success!");
	}
	return _size;
}
