#ifndef __PROTOCOL_UTIL_HPP__
#define __PROTOCOL_UTIL_HPP__

#include <iostream>
#include <string>
#include <json/json.h>

#define MESSAGE_SIZE 1024

typedef enum{
    LOGIN = 1,
    REGISTER,
    LOGOUT,
}request_type_t;

typede unsigned long id_t;

class Util{
    static void ValueToMessage(Json::Value &value_, std::string &message_)
    {
    #ifdef _MY_JSON_STYLE_
    	Json::StyledWriter write_;
    #else
    	Json::FastWriter write_;
    #endif
    	message_ = write_.write(value_);
    }
    
    static void MessageToValue(std::string &message_, Json::Value &value_)
    {
    	Json::Reader reader_;
    	reader_.parse(message_, value_, false);
    }
};

class LoginConnect{
    public:
        int sock;
        void *server;
    public:
        LoginConnect(int sock_, void *server_):sock(sock_), server(server_)
        {
        }
        ~LoginConnect()
        {}
};

struct RegisterInfo{
    char nick_name[32];
    char school[64];
    id_t id;
    char passwd[32];
};
struct LoginInfo{
    id_t id;
    char passwd[32];
};
struct Reply{
    int status;
    id_t id;
};

class Message{
	private:
		std::string nick_name;
		std::string school;
		std::string msg;
        id_t id;
	public:
		udp_data();
		~udp_data();

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

        void Serialize(std::string &message_)
        {
        	Json::Value value_;
        	value_["nick_name"] = nick_name;
        	value_["school"]    = school;
        	value_["msg"]       = msg;
        	value_["type"]      = type;
        	value_["cmd"]       = cmd;
            Util::ValueToMessage(value_, message_);
        }
        void Deserialize(std::string &message_)
        {
        	Json::Value value_;
            Util::MessageToValue(message_, value_);

        	nick_name = value_["nick_name"].asString();
        	school    = value_["school"].asString();
        	msg       = value_["msg"].asString();
        	type      = value_["type"].asString();
        	cmd       = value_["cmd"].asString();
        }
};

#endif


