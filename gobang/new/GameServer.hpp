#ifndef _GAMESERVER_HPP_
#define _GAMESERVER_HPP_

#include <iostream>
#include <pthread.h>
#include <unordered_map>
#include <unistd.h>
#include "Protocol.hpp"
#include "ThreadPool.hpp"

class Server{
    private:
        //任务管理
        int listen_sock;
        ThreadPool pool;
    public:
        Server():listen_sock(-1),pool(5)
        {}
        void InitServer()
        {
            listen_sock = SocketApi::Socket();
            SocketApi::Bind(listen_sock,SERVER_PORT);
            SocketApi::Listen(listen_sock);
        }
        ~Server()
        {
            if(listen_sock != -1){
                close(listen_sock);
                listen_sock = -1;
            }
        }
        static void HandlerMatch(int no_arg)
        {
            GameServer *p = Singleton::GetInstance();
            p->Match();
        }
        static void HandlerEvent(int sock)
        {
            //用短连接解决粘包问题
            char cmd;
            std::string msg;
            GameServer *p = Singleton::GetInstance();
            recv(sock, &cmd, 1, 0);
            switch(cmd){
                case 'r'://register
                    {
                        SocketApi::Recv(sock, msg);
                        Register_Request rr;
                        rr.Deserialize(msg);
                        int id = p->Register(rr.nick_name, rr.passwd);
                        Response r = {id, 0};
                        r.Serialize(msg);
                        SocketApi::Send(sock, msg);
                    }
                    break;
                case 'l'://login
                case 'o'://logout
                    {
                        SocketApi::Recv(sock, msg);
                        LL_Request lr;
                        lr.Deserialize(msg);
                        bool res = false;
                        if(cmd == 'l'){
                            res = p->Login(lr.player_id, lr.passwd);
                        }else{
                            res = p->Logout(lr.player_id, lr.passwd);
                        }
                        Response r;
                        r.player_id = lr.player_id;
                        if(res){
                            r.status = 0;
                        }else{
                            r.status = 1; // 登录&&退出失败
                        }
                        r.Serialize(msg);
                        SocketApi::Send(sock, msg);
                    }
                    break;
                case 'm'://match
                    {
                        SocketApi::Recv(sock, msg);
                        LL_Request lr;
                        lr.Deserialize(msg);
                        p->PushMatchPool(lr.player_id);
                        if(p->PlayerWait(lr.player_id)){
                            //match success

                        }else{

                        }
                    }
                    break;
                defalut:
                    {
                        //TODO
                    }
                    break;
            }
            close(sock);
        }
        void Start()
        {
            Task t;
            t.InitTask(HandlerMatch, -1);
            pool.AddTask(t);
            for ( ; ; ){
                int sock = SocketApi::Accept(listen_sock);
                if(sock < 0){
                    continue;
                }
                std::cout << "get a new client" << std::endl;
                Task t;
                t.InitTask(HandlerEvent, sock);
                pool.AddTask(t);
            }
        }
};
#endif

