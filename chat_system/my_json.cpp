#include "my_json.h"
my_json::my_json()
{
}

my_json::~my_json()
{
}

int my_json::serialize(Json::Value &_val, std::string &_out)
{
#ifdef _MY_JSON_STYLE_
	Json::StyledWriter _write;
#else
	Json::FastWriter _write;
#endif
	_out = _write.write(_val);

	return 0;

}

int my_json::deserialize(std::string &_str, Json::Value &_val_out)
{
	Json::Reader _reader;
	if (_reader.parse(_str, _val_out, false)){
		return 0;
	}else{
		return 1;
	}
}


