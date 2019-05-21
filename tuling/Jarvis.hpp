#ifndef _JARVIS_
#define _JARVIS_

#include <iostream>
#include <string>
#include <map>
#include "speech.h"

//语音识别Speech Recognition
class SpeechRec{
    private:
        std::string app_id;
        std::string api_key;
        std::string secret_key;
        aip::Speech *client;

    public:
        SpeechRec(const std::string &_app_id, const std::string _api_key,\
                const std::string &_secret_key)
            :app_id(_app_id),api_key(_api_key),secret_key(_secret_key)
        {
            client = new aip::Speech(app_id, api_key, secret_key);
        }
        //语音识别Automatic Speech Recognition
        void ASR()
        {
            std::map<std::string, std::string> options;
            std::string file_content;
            aip::get_file_content("./temp_file/demo.wav", &file_content);
            Json::Value result = client->recognize(file_content, "wav", 16000, options);
            std::cout << "语音识别结果: " << std::endl << result.toStyledString();
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

#endif












