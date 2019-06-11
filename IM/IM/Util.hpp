#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <iostream>
#include <json/json.h>
#include <memory>
#include "mongoose.h"

class Util{
    public:
        static std::string InfoToString(std::string &str)
        {
            std::string tmp = "\'";
            tmp += str;
            tmp += "\'";
            return tmp;
        }
        static std::string mr_str_to_string(struct mg_str *str)
        {
            std::string outstring;
            int len = str->len;
            for(auto i = 0; i < len; i++){
                outstring.push_back(str->p[i]);
            }
            return outstring;
        }
        static bool GetNameAndPasswd(std::string &_json_string,\
                std::string &_name, std::string &_passwd)
        {
            bool res = true;
            JSONCPP_STRING errs;
            Json::Value root;
            Json::CharReaderBuilder readerBuilder;
            std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
            res = jsonReader->parse(_json_string.c_str(), _json_string.c_str()+_json_string.length(), &root, &errs);
            if (!res || !errs.empty()) {
                std::cout << "parseJson err. " << errs << std::endl;
            }
            else{
                _name = root["name"].asString();
                _passwd = root["passwd"].asString();
            }
            return res;
        }
};

#endif
