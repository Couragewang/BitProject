#ifndef _MY_JSON_H_
#define _MY_JSON_H_

#include <iostream>
#include <json/json.h>

class my_json{
	public:
		my_json();
		~my_json();
		static int serialize(Json::Value &_val, std::string &_out);
		static int deserialize(std::string &_str, Json::Value &_val_out);
};

#endif
