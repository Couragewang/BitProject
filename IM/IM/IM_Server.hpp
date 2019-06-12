#ifndef _IM_SERVER_HPP_
#define _IM_SERVER_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <signal.h>
#include "mongoose.h"
#include "mysql.h"
#include "Util.hpp"

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

#define SESSION_TTL 600.0
#define SESSION_CHECK_INTERVAL 5.0
#define SESSION_NUM 128  //必须是确定大小的数组，不能超过！！
#define SESSION_COOKIE_NAME "IM"
#define SESSION_COOKIE_USER "NAME"

struct session{
    uint64_t id;
    double created;
    double last_used;
    std::string user;
};

//model
class IM_MysqlClient{
    private:
        MYSQL *conn;
    public:
        IM_MysqlClient()
        {
            conn = mysql_init(NULL);
            mysql_set_character_set(conn, "utf8");
            mysql_real_connect(conn, HOST, USER, PASSWD, DB, PORT, NULL, 0);
        }
        bool InsertUser(std::string &_name, std::string &_passwd, std::string &_status)
        {
            std::string sql = "INSERT INTO ";
            sql += USER_TB;
            sql += "(name, passwd, status) values (";
            sql += Util::InfoToString(_name);
            sql += ",md5(";
            sql += Util::InfoToString(_passwd);
            sql += "),";
            sql += Util::InfoToString(_status);
            sql += ")";
            int result = mysql_query(conn, sql.c_str());
            return result == 0 ? true : false;
        }
        bool SelectUser(std::string &_name, std::string &_passwd)
        {
            std::string sql = "SELECT * FROM user WHERE name=";
            sql += Util::InfoToString(_name);
            sql += " AND passwd=md5(";
            sql += Util::InfoToString(_passwd);
            sql += ")";
            bool result = false;
            if(0 == mysql_query(conn, sql.c_str())){
                MYSQL_RES *res = mysql_store_result(conn);
                if(mysql_num_rows(res) > 0){
                    std::cout <<"debug.....: " << mysql_num_rows(res) << std::endl;
                    result = true;
                }
                free(res);
            }
            return result;
        }
        void UpdateUserStatus(std::string &_name, std::string _status)
        {
            //TODO
            std::string sql;
        }
        void InsertMessage();
        void SelectMessage();
        ~IM_MysqlClient()
        {
            mysql_close(conn);
        }
};

class IM_Controller{
    public:
        IM_Controller()
        {
        }
        ~IM_Controller()
        {
        }
        bool AddUser(std::string _name,std::string _passwd, std::string _status)
        {
            IM_MysqlClient client;
            return client.InsertUser(_name, _passwd, _status);
        }
        bool IsUserLegal(std::string _name, std::string _passwd)
        {
            IM_MysqlClient client;
            return client.SelectUser(_name, _passwd);
        }
        void OnlineUser(std::string &name)
        {
            IM_MysqlClient client;
            return client.UpdateUserStatus(name, ONLINE);
        }
        void AddMessage()
        {
            //TODO
        }
        void SelectMessage()
        {
            //TODO
        }
};

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
        static struct session s_sessions[SESSION_NUM];
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
        static bool IsWebsocket(const struct mg_connection *nc)
        {
            return (nc->flags & MG_F_IS_WEBSOCKET) ? true : false;
        }
        static struct session* CreateSession(std::string &name)
        {
            int i = 0;
            for(; i < SESSION_NUM; i++){
                if(s_sessions[i].id == 0){
                    break;
                }
            }
            if( i < SESSION_NUM ){
                s_sessions[i].created = s_sessions[i].last_used = mg_time();
                s_sessions[i].user = name;
                s_sessions[i].id = (uint64_t)(mg_time()*1000000L);//采用时间戳作为session ID，防止冲突，本来应该用sha相关的算法生成，当时简单起见
                return &s_sessions[i];
            }
            return NULL;
        }
        static struct session* GetSession(struct http_message *hm)
        {
            //goto 不要越过变量的定义
            uint64_t sid = 0;
            int index = 0;
            char ssid[32];
            char *ssid_p = ssid;
            struct session *s = NULL;

            struct mg_str *cookie_header = mg_get_http_header(hm, "cookie");
            if(NULL == cookie_header){
                goto over;
            }
            if(!mg_http_parse_header2(cookie_header, \
                        SESSION_COOKIE_NAME, &ssid_p, sizeof(ssid))){
                goto over;
            }
            //字符串转长整型，16进制
            //sid = strtoull(ssid, NULL, 16);
            sid = strtoull(ssid, NULL, 10);

            for ( ; index < SESSION_NUM; index++){
                if( s_sessions[index].id == sid ){
                    s = &s_sessions[index];
                    s->last_used = mg_time();
                    goto over;
                }
            }

        over:
            if(ssid_p != ssid){
                free(ssid_p);
            }
            return s;
        }
        static void DestroySession(struct session *s)
        {
            s->id = 0; //只要将id设置为0，即代表该session失效
        }
        //定期检查session是否超时，超时的话，就移除该session
        static void CheckSessions(void)
        {
            //获取当前时间，减去session生命周期
            double threadhold = mg_time() - SESSION_TTL;
            for(auto i = 0; i < SESSION_NUM; i++){
                if(s_sessions[i].id != 0 && s_sessions[i].last_used < threadhold){
                    std::cerr << "Session: " << s_sessions[i].id << " User: " << s_sessions[i].user << " idle long time...close" << std::endl;
                    DestroySession(&s_sessions[i]);
                }
            }
        }
        static void Broadcast(struct mg_connection *nc, std::string info)
        {
            struct mg_connection *c;
            for(c = mg_next(nc->mgr, NULL); c != NULL; c = mg_next(nc->mgr, c)){
                //if(nc == c) continue;
                mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, info.c_str(), info.size());
            }
        }
        static void QuitUser(struct mg_connection *nc, std::string info)
        {
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, info.c_str(), info.size());
            nc->flags |= MG_F_SEND_AND_CLOSE;
        }
        static void LoginHandler(struct mg_connection *nc, int ev, void *ev_data)
        {
            if(ev != MG_EV_HTTP_REQUEST){ //mongoose 样例的bug，链接关闭的事件也会触发...
                return;
            }
            struct http_message *hm = (struct http_message*)ev_data;
            if(mg_vcmp(&hm->method, "POST") != 0){
                mg_serve_http(nc, hm, http_opts);
            }
            else{
                int result = 0;
                std::string name;
                std::string passwd;
                std::string user_info = Util::mr_str_to_string(&hm->body);
                if(Util::GetNameAndPasswd(user_info, name, passwd) && !name.empty() && !passwd.empty()){
                    if(ctr.IsUserLegal(name, passwd)){
                        struct session *s = CreateSession(name);
                        if(s){
                            std::stringstream ss;
                            ss << "Set-Cookie: " << SESSION_COOKIE_NAME << "=" << s->id << "; path=/\r\n";
                            ss << "Set-Cookie: " << SESSION_COOKIE_USER << "=" << s->user << "; path=/";
                            std::string shead = ss.str();

                            mg_printf(nc, "HTTP/1.1 200 OK\r\n%*s\r\n\r\n",shead.size(), shead.c_str());
                            mg_printf(nc, "{\"result\": %d}\r\n", result);
                        }
                        else{
                            //TODO
                        }
                    }
                    else{
                        result = 1;
                        mg_printf(nc, "HTTP/1.1 403 Unanthorized\r\n\r\n");
                        mg_printf(nc, "{\"result\": %d}\r\n", result);
                    }
                }
                else{
                    result = 2;
                    mg_printf(nc, "HTTP/1.1 400 Bad Request\r\n\r\n");
                    mg_printf(nc, "{\"result\": %d}\r\n", result);
                }
                nc->flags |= MG_F_SEND_AND_CLOSE; //相应完毕，完毕链接
            }
        }

        static void SigninHandler(struct mg_connection *nc, int ev, void *ev_data)
        {
            if(ev != MG_EV_HTTP_REQUEST){ //mongoose 样例的bug，链接关闭的事件也会触发...
                return;
            }
        //    std::cout << "aaaaa" <<std::endl;
            struct http_message *hm = (struct http_message*)ev_data;
            if(mg_vcmp(&hm->method, "POST") != 0){
                mg_serve_http(nc, hm, http_opts);
            }
            else{
                int result = 0;
                std::string name;
                std::string passwd;
                std::string user_info = Util::mr_str_to_string(&hm->body);
                if(Util::GetNameAndPasswd(user_info, name, passwd) && !name.empty() && !passwd.empty()){
                    std::string status = OFFLINE;
                    if(!ctr.AddUser(name, passwd, status)){
                        result = 1; //插入失败，已经存在该用户
                    }
                    mg_printf(nc, "HTTP/1.1 200 OK\r\n\r\n");
                }
                else{
                    result = 2; //请求有错误
                    mg_printf(nc, "HTTP/1.1 400 Bad Request\r\n\r\n");
                }
                mg_printf(nc, "{\"result\": %d}", result);
                nc->flags |= MG_F_SEND_AND_CLOSE;
            }
        }
        static void EventHandler(struct mg_connection *nc, int ev, void *ev_data)
        {
            switch(ev){
                case MG_EV_HTTP_REQUEST:{
                        struct http_message *hm = (struct http_message*)ev_data;
                        struct session *s = GetSession(hm);
                        if(!s){
                            //获取session失败，说明用户从未登录过,重定向到登录页面
                            std::cerr << "request index.html, no session, redirect login.html" <<std::endl;
                            mg_http_send_redirect(nc, 302, mg_mk_str("/login.html"), mg_mk_str(NULL));
                            nc->flags |= MG_F_SEND_AND_CLOSE;
                            break; //重定向完毕，退出事件处理逻辑，关闭链接
                        }
                        //获取session成功，保存
                        std::cerr << "have default session, login success" << std::endl;
                        //nc->user_data = s;
                        //std::cout << "login nc addr: " << nc << std::endl;
                        mg_serve_http(nc, hm, http_opts);
                    }
                    break;
                case MG_EV_WEBSOCKET_HANDSHAKE_DONE:{
                        //struct session *s = (struct session*)nc->user_data;
                        //std::cout << "Done nc addr: " << nc << std::endl;
                        //std::string tips = s->user;
                        std::string tips = "... 有新人加入，大家欢迎...";
                        Broadcast(nc, tips);
                    }
                    break;
                case MG_EV_WEBSOCKET_FRAME:{
                        struct websocket_message *wm = (struct websocket_message*)ev_data;
                        struct mg_str d = {(char *) wm->data, wm->size};
                        std::string message = Util::mr_str_to_string(&d);
                        Broadcast(nc, message);
                    }
                    break;
                case MG_EV_CLOSE:{
                        if(IsWebsocket(nc)){
                            std::string message = "...有用户退出...";
                            Broadcast(nc, message);
                        }
                    }
                    break;
                case MG_EV_TIMER:{
                        CheckSessions();
                        //注意，能引起超时事件的只有listen绑定的链接
                        //这里就保证系统能定期检查是否有session超时
                        mg_set_timer(nc, mg_time() + SESSION_CHECK_INTERVAL);
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
            mg_register_http_endpoint(nc, "/login.html", IM_Server::LoginHandler);

            mg_register_http_endpoint(nc, "/signin.html", IM_Server::SigninHandler);
            //mg_register_http_endpoint(nc, "/logout.html", IM_Server::LoginHandler);
            mg_set_timer(nc, mg_time() + SESSION_CHECK_INTERVAL);
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

//静态变量初始化
volatile bool IM_Server::quit = false;
IM_Controller IM_Server::ctr;
struct session IM_Server::s_sessions[SESSION_NUM] = {0};

#endif

