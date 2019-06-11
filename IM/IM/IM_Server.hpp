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
#define SESSION_NUM 64
#define SESSION_COOKIE_NAME "IM"

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
        static std::vector<struct session> s_sessions;
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
        static struct session& CreateSession(std::string &name)
        {
            struct session s;
            s.created = s.last_used = mg_time();
            s.user = name;
            s.id = (uint64_t)(mg_time()*1000000L);//采用时间戳作为session ID，防止冲突，本来应该用sha相关的算法生成，当时简单起见
            std::cout << "debug: " << s.id <<std::endl;
            s_sessions.push_back(s);
            return s_sessions[s_sessions.size()-1];
        }
        static int GetSession(struct http_message *hm)
        {
            //goto 不要越过变量的定义
            uint64_t sid = 0;
            int index = 0;
            int size = 0;
            char ssid[32];
            char *ssid_p = ssid;

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
            std::cout << "client: " << sid <<std::endl;
            size = s_sessions.size();
            for ( ; index < size; index++){
                if( s_sessions[index].id == sid ){
                    s_sessions[index].last_used = mg_time();
                    goto over;
                }
            }

        over:
            if(ssid_p != ssid){
                free(ssid_p);
            }
            if(index == size){
                index = -1;
            }
            return index;
        }
        //定期检查session是否超时，超时的话，就移除该session
        static void CheckSessions(void)
        {
            //获取当前时间，减去session生命周期
            double threadhold = mg_time() - SESSION_TTL;
            auto it = s_sessions.begin();
            for(; it != s_sessions.end(); ){
                //当前的用户是合法用户，并且超时了,就要删除它，注意序列式容器的迭代器失效的问题
                if(it->id != 0 && it->last_used < threadhold){
                    std::cout << "Session: " << it->id << " User: " << it->user << " idle long time...close" << std::endl;
                    it = s_sessions.erase(it);
                }
                else{
                    ++it;
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
                        auto s = CreateSession(name);
                        std::stringstream ss;
                        ss << "Set-Cookie: " << SESSION_COOKIE_NAME << "=" << s.id << "; path=/";
                        std::string shead = ss.str();
                        mg_http_send_redirect(nc, 302, mg_mk_str("/"), mg_mk_str(shead.c_str()));
                        std::cout << s.user << " Logged In, sid : " << s.id << std::endl;
                    }
                    else{
                        result = 1;
                        mg_printf(nc, "HTTP/1.0 403 Unanthorized\r\n\r\n");
                        mg_printf(nc, "{\"result\": %d}\r\n", result);
                    }
                    //mg_printf(nc, "HTTP/1.0 200 OK\r\n\r\n");
                }
                else{
                    result = 2;
                    mg_printf(nc, "HTTP/1.0 400 Bad Request\r\n\r\n");
                    mg_printf(nc, "{\"result\": %d}\r\n", result);
                }
                nc->flags |= MG_F_SEND_AND_CLOSE; //相应完毕，完毕链接
            }
            (void)ev;//防止未使用变量引起的警告
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
                    mg_printf(nc, "HTTP/1.0 200 OK\r\n\r\n");
                }
                else{
                    result = 2; //请求有错误
                    mg_printf(nc, "HTTP/1.0 400 Bad Request\r\n\r\n");
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
                        int index = GetSession(hm);
                        if(index < 0){
                            //获取session失败，说明用户从未登录过,重定向到登录页面
                            std::cout << "获取session失败，说明用户从未登录过,重定向到登录页面" <<std::endl;
                            mg_http_send_redirect(nc, 302, mg_mk_str("/login.html"), mg_mk_str(NULL));
                            nc->flags |= MG_F_SEND_AND_CLOSE;
                            break; //重定向完毕，退出事件处理逻辑，关闭链接
                        }
                        //获取session成功，保存
                        std::cout << "获取session成功，保存" << std::endl;
                        nc->user_data = new int(index);
                        mg_serve_http(nc, hm, http_opts);
                        //mg_http_send_redirect(nc, 302, mg_mk_str("/"), mg_mk_str(NULL));
                    }
                    break;
                case MG_EV_WEBSOCKET_HANDSHAKE_DONE:{
                        int index = *((int*)nc->user_data);
                        std::string name = s_sessions[index].user;
                        std::string tips = name;
                        tips += "++ join";
                        Broadcast(nc, tips);
                    }
                    break;
                case MG_EV_WEBSOCKET_FRAME:{
                        int index = *((int*)nc->user_data);
                        std::string name = s_sessions[index].user;

                        struct websocket_message *wm = (struct websocket_message*)ev_data;
                        struct mg_str d = {(char *) wm->data, wm->size};
                        std::string message = Util::mr_str_to_string(&d);
                        name += ">>> ";
                        name += message;
                        Broadcast(nc, name);
                    }
                    break;
                case MG_EV_CLOSE:{
                        if(IsWebsocket(nc)){
                            int *p = (int*)nc->user_data;
                            std::string name = s_sessions[*p].user;
                            std::string tips = name;
                            tips += " ++ left";
                            Broadcast(nc, tips);
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
std::vector<struct session> IM_Server::s_sessions(SESSION_NUM);

#endif

