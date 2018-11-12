#ifndef _UDP_DATA_H_
#define _UDP_DATA_H_

#include <iostream>
#include "my_json.h"

extern const int BUF_SIZE;//

class udp_data{
	public:
		udp_data();
		~udp_data();
		int val_to_str(std::string &_out);
		int str_to_val(std::string &_in);

		const std::string& get_nick_name(){return this->nick_name;}
    	const std::string& get_school(){return this->school;}
    	const std::string& get_msg(){return this->msg;}
    	const std::string& get_type(){return this->type;}
    	const std::string& get_cmd(){return this->cmd;}

		void set_nick_name(const std::string &_nick_name)
		{this->nick_name = _nick_name;}
		void set_school(const std::string &_school)
		{this->school = _school;}
		void set_msg(const std::string &_msg)
		{this->msg = _msg;}
		void set_type(const std::string &_type)
		{this->type = _type;}
		void set_cmd(const std::string &_cmd)
		{this->cmd = _cmd;}

	private:
		my_json json_bit;

		std::string nick_name;
		std::string school;
		std::string msg;
		std::string type;
		std::string cmd;
};
#endif

