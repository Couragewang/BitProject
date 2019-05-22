#include <iostream>
#include <string>
#include <httplib.h>
#include <json/json.h>
//#include <json.h>

using namespace std;

int main()
{
    string input;
    while(1){
        cout << "Please Enter: ";
        cin >> input;
        Json::Value root;
        //Json::FastWriter w;
        Json::StyledWriter w;
        root["reqType"] = 0;

        Json::Value InputText;
        InputText["text"] = input;
        Json::Value Perception;
        Perception["inputText"] = InputText;
        root["perception"] = Perception;
        Json::Value UserInfo;
        UserInfo["apiKey"] = "76637bcc4aec472ea53f3647482bcb29";
        UserInfo["userId"] = 1;
        root["userInfo"] = UserInfo;
        string param = w.write(root);
        httplib::Client cli("openapi.tuling123.com", 80);
        auto res = cli.Post("/openapi/api/v2", param, "application/json");
        if(res && res->status == 200){
            Json::Value root;
            Json::Reader r;
            r.parse(res->body, root, false);
            Json::Value Intent = root["intent"];
        //    cout << Intent["code"].asInt() << endl;
        //    cout << Intent["intentName"].asString() << endl;
            Json::Value results = root["results"];
            for(auto i = 0; i < results.size(); i++){
                Json::Value values = results[i]["values"];
                cout <<"Robot: "<< values["text"];
            }
        }
        else{
            cout << "error" << endl;
        }
    }
    return 0;
}
