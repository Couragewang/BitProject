#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>

int main()
{
    std::unordered_map<std::string, std::string> m{
        {
            ".html","text/html"
        },
        {
            ".css", "text/css"
        }
    };
    std::cout << m[".css"] << std::endl;
//    std::string s("\n");
//    s.substr(2);

    //std::string s;
    //std::string s1 = "abcd";
    //std::string s2 = "1234";
    //std::string s3 =  "-=00";

    //s = s1 + s2 + s3;

    //std::cout << s << std::endl;
   // int num = 100;
   // std::stringstream ss;
   // ss << num << "hello";
   // std::cout << ss.str() << std::endl;
   // ss.str("");
   // ss << 200;
   // std::string str = ss.str();
   // std::cout << str << std::endl;
   // ss.str("");
   // ss << 300;
   // str = ss.str();
   // std::cout << str << std::endl;


    //std::stringstream ss;
    //int num = 10;
    //ss << num;
    //std::cout << ss.str() << std::endl;
    //std::unordered_map<std::string, std::string> m{
    //    {"a","b"},
    //    {"c","d"},
    //};
    //std::cout << m["a"] << std::endl;
    //std::string str = "test.cpp";
    //std::size_t _pos = str.find_last_of('.');
    //if(_pos == std::string::npos){
    //    std::cout << "hello world" << std::endl;
    //}else{
    //    std::cout << "aaaaaaaaaaaa"<<std::endl;
    //}
    ////std::cout << str.substr(0, _pos) << std::endl;
    //std::cout << str.substr(_pos+1) << std::endl;
    return 0;
}
