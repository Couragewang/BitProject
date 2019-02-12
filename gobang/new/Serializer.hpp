#ifndef __SERIALIZER_HPP__
#define __SERIALIZER_HPP__

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <json/json.h>

//基础序列化和反序列化
class Util{
    public:
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
            std::cout << tips_;
            std::cin >> passwd_;
        }
};
#endif










