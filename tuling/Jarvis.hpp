#ifndef _JARVIS_
#define _JARVIS_

#include <iostream>
#include <cstdio>
#include <string>
#include <map>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unordered_map>
#include "speech.h"
#include "base/http.h"//直接使用百度提供的httpclient
//#include <httplib.h>

#define SPEECH_FILE "temp_file/demo.wav"

//语音识别Speech Recognition
class SpeechRec{
    private:
        std::string app_id="16296264";
        std::string api_key="xNkCCoo4Bt7GGIhGvTlR1BB6";
        std::string secret_key="f4BoknKiDUeHmbsHD9ctzBeKHliCtLed";
        aip::Speech *client;

    public:
        SpeechRec()
        {
            client = new aip::Speech(app_id, api_key, secret_key);
        }
        //语音识别Automatic Speech Recognition
        void ASR(int &err_code, std::string &message)
        {
            std::cout << std::endl << "...正在识别..." << std::endl;
            std::map<std::string, std::string> options; //以中文形式显示
            options["dev_pid"] = "1536";
            options["lan"] = "ZH";

            std::string file_content;
            aip::get_file_content(SPEECH_FILE, &file_content);
            Json::Value result = client->recognize(file_content, "wav", 16000, options);
            err_code = result["err_no"].asInt();
            if(err_code == 0){
              //  std::cout << result.toStyledString() << std::endl;
                message = result["result"][0].asString();
              //  std::cout << "message : " << message<< std::endl;
            }
            else{
                message="...识别错误...";
            }
            //std::cout << "语音识别结果: " << std::endl << result.toStyledString();
        }
        //语音合成Text To Speech
        void TTS()
        {
            std::ofstream ofile;
        }
        ~SpeechRec()
        {
            delete client;
            client = NULL;
        }
};

//采用图灵机器人，进行智能对话
class InterRobot{
    private:
        std::string url="openapi.tuling123.com/openapi/api/v2";
        std::string api_key="76637bcc4aec472ea53f3647482bcb29";
        std::string user_id = "1";
        aip::HttpClient client;
    public:
        InterRobot()
        {}
        Json::Value PostRequest(Json::Value data)
        {
            std::string response;
            Json::Value obj;
            Json::CharReaderBuilder crbuilder;
            int code = this->client.post(url, nullptr, data, nullptr, &response);
            if(code != CURLcode::CURLE_OK){
                obj[aip::CURL_ERROR_CODE]=code;
                return obj;
            }
            std::string error;
            std::unique_ptr<Json::CharReader> reader(crbuilder.newCharReader());
            reader->parse(response.data(), response.data() + response.size(), &obj, &error);
            return obj;
        }
        void Talk(std::string &message)
        {
            Json::Value root;
            Json::Value item1;
            Json::Value item2;
            root["reqType"] = 0;
            item1["text"] = message;
            item2["inputText"] = item1;
            item1.clear();
            root["perception"] = item2;
            item2.clear();
            item2["apiKey"] = api_key;
            item2["userId"] = user_id;
            root["userInfo"] = item2;
            item2.clear();

            //std::cout << root.toStyledString() << std::endl;
            Json::Value ret = PostRequest(root);
            //std::cout << result.toStyledString() << std::endl;
            //Json::Value Intent = result["intent"];
            Json::Value _result = ret["results"];
            for(auto i = 0; i < _result.size(); i++){
                Json::Value values = _result[i]["values"];
                std::cout <<"Robot: "<< values["text"] << std::endl;
            }

            //std::cout << result.toStyledString() << std::endl;
        }
        ~InterRobot()
        {}
};

class Jarvis{
    private:
        SpeechRec sr;
        InterRobot robot;
        std::unordered_map<std::string, std::string> command_set;
    public:
        Jarvis()
        {
            command_set.insert(std::make_pair("查看内存。", "free"));
            command_set.insert(std::make_pair("显示当前文件。", "ls -al"));
            command_set.insert(std::make_pair("查看硬盘信息。", "df -h"));
            command_set.insert(std::make_pair("关闭防火墙。", "systemctl stop firedwall"));
            command_set.insert(std::make_pair("打开防火墙。", "systemctl start firedwall"));
        }
        bool Exec(std::string command)
        {
            FILE *fp = popen(command.c_str(), "r");
            if(NULL == fp){
                std::cerr << "popen error!" << std::endl;
                return false;
            }
            pclose(fp);
            return true;
        }
        bool MessageIsCommand(std::string _message, std::string &cmd)
        {
            std::unordered_map<std::string, std::string>::iterator iter = command_set.find(_message);
            if(iter != command_set.end())
            {
                cmd = iter->second;
                return true;
            }
            cmd = "";
            return false;
        }
        bool RecordAndASR(std::string &message)
        {
            int err_code = -1;
            std::string record = "arecord -t wav -c 1 -r 16000 -d 3 -f S16_LE ";
            record += SPEECH_FILE;
            record += ">/dev/null 2>&1"; //不显示输出结果或者消息

            std::cout << "...请讲话...";
            fflush(stdout);
            if(Exec(record)){
                sr.ASR(err_code, message);
                if(err_code == 0){
                    return true;
                }
                std::cout << "语音识别失败..." << std::endl;
            }
            else{
                std::cout << "录制失败..." << std::endl;
            }
            return false;
        }
        void Run()
        {
            volatile bool quit = false;
            std::string message;
            while(!quit){
                message="";
                bool ret = RecordAndASR(message);
                if(ret){
                    std::string cmd;
                    std::cout << "我: " << message << std::endl;
                    if(MessageIsCommand(message, cmd)){
                        std::cout << cmd << std::endl;
                        //Exec(cmd);
                    }
                    else{
                        robot.Talk(message);
                    }
                }
            }
        }
        ~Jarvis()
        {}
};

#endif
