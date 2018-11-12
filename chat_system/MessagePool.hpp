#ifndef _DATA_POOL_H_
#define _DATA_POOL_H_

#include <iostream>
#include <string>
#include <vector>
#include <semaphore.h>

#define MESSAGE_POOL_SIZE 512

class MessagePool{
	private:
		const int pool_size;
		std::vector<std::string> pool;
		sem_t product_sem;
		sem_t consume_sem;

		int p_step;
		int c_step;
	public:
        data_pool(int size_ = MESSAGE_POOL_SIZE):pool_size(size_),pool(size_)
        {
        	p_step = c_step = 0;
        	sem_init(&product_sem, 0, size_);
        	sem_init(&consume_sem, 0, 0);
        }
        bool GetMessage(std::string &message_)
        {
        	sem_wait(&consume_sem);
        	message_ = pool[c_step];
        	sem_post(&product_sem);
            c_step++;
        	c_step %= pool_size;
        }
        
        bool PutMessage(const std::string &message_)
        {
        	sem_wait(&product_sem);
        	pool[p_step] = message_;
        	sem_post(&consume_sem);
            p_step++;
        	p_step %= pool_size;
        }
        ~data_pool()
        {
        	sem_destroy(&product_sem);
        	sem_destroy(&consume_sem);
        }
};

#endif


#include "udp_data.h"

const int BUF_SIZE = 1024;//

udp_data::udp_data()
{
}

udp_data::~udp_data()
{
}

int udp_data::val_to_str(std::string &_out)
{
	Json::Value _val;
	_val["nick_name"] = this->nick_name;
	_val["school"]    = this->school;
	_val["msg"]       = this->msg;
	_val["type"]      = this->type;
	_val["cmd"]       = this->cmd;
	json_bit.serialize(_val, _out);
}

int udp_data::str_to_val(std::string &_in)
{
	Json::Value _val;
	json_bit.deserialize(_in, _val);
	this->nick_name = _val["nick_name"].asString();
	this->school    = _val["school"].asString();
	this->msg       = _val["msg"].asString();
	this->type      = _val["type"].asString();
	this->cmd       = _val["cmd"].asString();
}

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

