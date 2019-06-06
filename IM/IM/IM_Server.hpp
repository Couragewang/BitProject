#ifndef _IM_SERVER_HPP_
#define _IM_SERVER_HPP_

#include <iostream>
#include <string>
#include <signal.h>
#include "mongoose.h"
#include "mysql.h"

#define WEB_ROOT "web"
#define OFFLINE "offline"
#define ONLINE "online"

#define HOST       "localhost"
#define USER       "root"
#define PASSWD     ""
#define DB         "IM_2019_6_5"
#define PORT       3306
#define USER_TB    "user"
#define MESSAGE_TB "message"

static std::string InfoToString(std::string &str)
{
    std::string tmp = "\'";
    tmp += str;
    tmp += "\'";
    return tmp;
}

//model
class IM_MysqlClient{
    private:
        MYSQL *conn;
    public:
        IM_MysqlClient()
        {
            conn = mysql_init(NULL);
            mysql_real_connect(conn, HOST, USER, PASSWD, DB, PORT, NULL, 0);
        }
        bool InsertUser(std::string &_name, std::string &_passwd, std::string &_status)
        {
            std::string sql = "INSERT INTO ";
            sql += USER_TB;
            sql += "(name, passwd, status) values (";
            sql += InfoToString(_name);
            sql += ",";
            sql += InfoToString(_passwd);
            sql += ",";
            sql += InfoToString(_status);
            sql += ")";
            int result = mysql_query(conn, sql.c_str());
            return result == 0 ? true : false;
            //for debug
            //std::cout << "result:" << result <<" sql:" << sql << std::endl;
        }
        bool SelectUser(std::string &_name, std::string &_passwd)
        {
            return true;
        }
        void UpdateUserStatus();
        void InsertMessage();
        void SelectMessage();
        ~IM_MysqlClient()
        {
            mysql_close(conn);
        }
};

class IM_Controller{
    private:
        IM_MysqlClient client;
    public:
        IM_Controller()
        {
        }
        ~IM_Controller()
        {
        }
        bool AddUser(std::string _name,std::string _passwd, std::string _status)
        {
            return client.InsertUser(_name, _passwd, _status);
        }
        bool IsUserLegal(std::string _name, std::string _passwd)
        {
            return client.SelectUser(_name, _passwd);
        }
        void SetUserStatus();
        void AddMessage();
        void SelectMessage();
};

static std::string mr_str_to_string(struct mg_str *str)
{
    std::string outstring;
    for(auto i = 0; i < str->len; i++){
        outstring.push_back(str->p[i]);
    }
    return outstring;
}
//http server选项
struct mg_serve_http_opts http_opts;
class IM_Server{
    private:
        std::string port;
        static volatile bool quit;
        //mongoose 事件管理器
        struct mg_mgr mgr;
        //listen socket 对应的connect
        struct mg_connection *nc;
        //访问用户数据库
        static IM_Controller ctr;
    public:
        IM_Server(std::string _port = "8080")
            :port(_port)
        {
            http_opts.document_root= WEB_ROOT;
            http_opts.enable_directory_listing="yes";
        }
        static void ExitServer(int sig)
        {
            quit = true;
            std::cout << "IM Server Close ... Done" << std::endl;
        }
        static void ProcessRegister(struct mg_connection *nc, struct http_message *hm)
        {
            //for test
            char name[64];
            char passwd[128];
            memset(name,0, sizeof(name));
            memset(passwd,0, sizeof(passwd));
            mg_get_http_var(&hm->body, "name", name, sizeof(name));
            mg_get_http_var(&hm->body, "passwd", passwd, sizeof(passwd));

            std::string _name = name;
            std::string _passwd = passwd;
            std::string _status = OFFLINE;
            //for test
            _name = "none";
            _passwd = "none";
            if(ctr.AddUser(_name, _passwd, _status)){

            }
        }
        static bool ProcessLogin(struct mg_connection *nc, struct http_message *hm)
        {
            //for test
            std::string name = "zhangsan";
            std::string passwd="12345";
            return ctr.IsUserLegal(name, passwd);
        }
        static void Broadcast(struct mg_connection *nc, std::string info)
        {
            struct mg_connection *c;
            for(c = mg_next(nc->mgr, NULL); c != NULL; c = mg_next(nc->mgr, c)){
                //if(nc == c) continue;
                mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, info.c_str(), info.size());
            }
        }
        static bool IsWebsocket(const struct mg_connection *nc)
        {
            return (nc->flags & MG_F_IS_WEBSOCKET) ? true : false;
        }
        static void HandlerResponse(struct mg_connection *nc, struct http_message *hm)
        {
            //std::cout << (char*)hm->body.p <<std::endl;
        }
        static void EventHandler(struct mg_connection *nc, int ev, void *ev_data)
        {
            switch(ev){
                case MG_EV_HTTP_REQUEST:{
                        struct http_message *hm = (struct http_message*)ev_data;
                        if(mg_vcmp(&hm->uri, "/register") == 0){
                            ProcessRegister(nc, hm);
                            //HandlerResponse(nc, hm);
                        }
                        else if(mg_vcmp(&hm->uri, "/login") == 0 && ProcessLogin(nc, hm)){
                            //std::cout << "uri:"<< hm->uri.p <<" hello login" << std::endl;
                        }
                        else{
                            mg_serve_http(nc, hm, http_opts);
                        }
                    }
                    break;
                case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
                    Broadcast(nc, "any body ++ join");
                    break;
                case MG_EV_WEBSOCKET_FRAME:{
                        struct websocket_message *wm = (struct websocket_message*)ev_data;
                        std::string message = (char*)wm->data;
                        Broadcast(nc, message);
                    }
                    break;
                case MG_EV_CLOSE:{
                        if(IsWebsocket(nc)){
                            Broadcast(nc, "any body ++ left");
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        void InitServer()
        {
            signal(SIGINT, IM_Server::ExitServer);
            mg_mgr_init(&mgr, NULL);
            nc = mg_bind(&mgr, port.c_str(), IM_Server::EventHandler);
            mg_set_protocol_http_websocket(nc);
        }
        void Run()
        {
            int timeout = 20000;
            std::cout << "IM Server Start Port: " << port << " ... Done" << std::endl;
            while(!quit){
                mg_mgr_poll(&mgr, timeout);
            }
            mg_mgr_free(&mgr);
        }
};
volatile bool IM_Server::quit = false;
IM_Controller IM_Server::ctr;
#endif
