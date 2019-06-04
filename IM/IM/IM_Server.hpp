#ifndef _IM_SERVER_HPP_
#define _IM_SERVER_HPP_

#include <iostream>
#include <string>
#include <signal.h>
#include "mongoose.h"

#define WEB_ROOT "web"

class IM_Server{
    private:
        std::string port;
        static volatile bool quit;
        //http server选项
        struct mg_serve_http_opts http_opts;
        //mongoose 事件管理器
        struct mg_mgr mgr;
        //listen socket 对应的connect
        struct mg_connection *nc;
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
            struct http_message *hm = (struct http_message*)ev_data;
            std::cout << (char*)hm->body.p << std::endl;
        }
        static void EventHandler(struct mg_connection *nc, int ev, void *ev_data)
        {
            switch(ev){
                case MG_EV_HTTP_REQUEST:
                    {
                        if(mg_vcmp(&hm->uri, "/register") == 0){
                            ProcessRegister();
                        }
                        else if(mg_vcmp(&hm->uri, "/login") == 0){
                            //ProcessLogin();
                        }
                        else{
                            std::cout << "unknow" << std::endl;
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
#endif


