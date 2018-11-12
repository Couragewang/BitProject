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

