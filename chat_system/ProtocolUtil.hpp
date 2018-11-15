#ifndef __PROTOCOL_UTIL_HPP__
#define __PROTOCOL_UTIL_HPP__

#include <iostream>
#include <string>
#include <json/json.h>

#define MESSAGE_SIZE 1024

#define LOGIN    1
#define REGISTER 2
#define LOGOUT   3

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

    //此处代码可以在优化，设置密码输入不回显功能
    static void EnterPasswd(std::string tips_, std::string &passwd_)
    {
        std::cout << tips;
        std::cin >> passwd_;
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
		Message();
		~Message();

		const std::string& GetNickName(){return this->nick_name;}
    	const std::string& GetSchool(){return this->school;}
    	const std::string& GetMsg(){return this->msg;}
    	const id_t& GetId(){return this->id;}

		void SetNickName(const std::string &n_)
		{this->nick_name = n_;}
		void SetSchool(const std::string &s_)
		{this->school = s_;}
		void SetMsg(const std::string &m_)
		{this->msg = m_;}
        void SetId(const id_t &i_)
        {this->id = i_;}

        void Serialize(std::string &message_)
        {
        	Json::Value value_;
        	value_["nick_name"] = nick_name;
        	value_["school"]    = school;
        	value_["msg"]       = msg;
            value_["id"]        = id;
            Util::ValueToMessage(value_, message_);
        }
        void Deserialize(std::string &message_)
        {
        	Json::Value value_;
            Util::MessageToValue(message_, value_);

        	nick_name = value_["nick_name"].asString();
        	school    = value_["school"].asString();
        	msg       = value_["msg"].asString();
        	id       = value_["id"].asInt();
        }
};

#endif


