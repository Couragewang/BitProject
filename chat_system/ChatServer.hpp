#ifndef __CHAT_SERVER_HPP__
#define __CHAT_SERVER_HPP__

#include <iostream>
#include <string>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "ProtocolUtil.hpp"
#include "MessagePool.hpp"
#include "UserManager.hpp"
#include "Log.hpp"

class ChatServer{
    private:
        int sock;
        int port;
        int login_sock;
        int login_port;
        MessagePool pool;
        UserManager user_manager;

    public:
        //ChatServer(int login_port_, int port_):sock(-1), login_port(login_port_), port(port_)
        //{}
        ChatServer():sock(-1), login_port(8080), port(8081)
        {}
        void InitServer()
        {
	        sock = socket(AF_INET, SOCK_DGRAM, 0);
	        if(this->sock < 0){
	        	LOG(ERROR, "create sock error!");
	        	exit(2);
	        }
	        LOG(INFO, "create sock success!");

	        struct sockaddr_in local_;
	        bzero(&local_, sizeof(local_));

	        local_.sin_family = AF_INET;
	        local_.sin_port   = htons(port);
	        local_.sin_addr.s_addr = htonl(INADDR_ANY);
	        socklen_t len_ = sizeof(struct sockaddr_in);

	        if( -1 == bind(sock, (struct sockaddr*)&local_, len_)){
	        	LOG(ERROR, "bind sock error!");
                exit(3);
	        }
	        LOG(INFO, "bind sock success!");
	        LOG(INFO, "initialize server success!");

            login_sock = socket(AF_INET, SOCK_STREAM, 0);
            if(login_sock < 0){
	        	LOG(ERROR, "create login sock error!");
                exit(4);
            }
            int opt = 1;
            setsockopt(login_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            local_.sin_port = htons(login_port);
            if(-1 == bind(login_sock, (struct sockaddr*)&local_, len_)){
	        	LOG(ERROR, "bind login sock error!");
                exit(5);
            }
            if(-1 == listen(login_sock, 5)){
	        	LOG(ERROR, "bind login sock error!");
                exit(6);
            }
        }
        UserManager &GetUserManager()
        {
            return user_manager;
        }
        void RecvMessage(std::string &message_)
        {
	        char msg_[MESSAGE_SIZE];

	        struct sockaddr_in client_;
	        socklen_t len_ = sizeof(client_);
	        bzero(&client_, len_);

	        ssize_t size_ = recvfrom(sock,msg_, sizeof(msg_), 0, (struct sockaddr *)&client_, &len_);
	        if( -1 == size_){
	        	LOG(ERROR, "recv data from client failed!");
	        }
            else{
	        	LOG(INFO, "recv data from client success!");
	        	LOG(INFO, msg_);
	        	message_ = msg_;
                Message client_message_;
                client_message_.Deserialize(message_);
                if(user_manager.SafeCheck(client_message_.GetId(), client_, len_)){
                    pool.PutMessage(message_);
                }
                else{
                    LOG(WARNING, "illegal user try send message!");
                }
	        }
        }
        void SendMessage(const std::string &message_, struct sockaddr_in &client_, socklen_t &len_)
        {
	        ssize_t size_ = sendto(sock, message_.c_str(), message_.size(), 0, (struct sockaddr*)&client_, len_);
	        if( size_ < 0 ){
	        	LOG(ERROR, "send data to client failed!");
	        }else{
	        	LOG(INFO, "send data to client success!");
	        	LOG(INFO, message_);
                //std::cout << inet_ntoa(client_.sin_addr) << " : " << htons(client_.sin_port) << std::endl;
	        }
        }
        int BroadcastMessage()
        {
            std::string message_;
            pool.GetMessage(message_);
            std::vector<User>&u_ = user_manager.GetOnlineUser(); //bug?
            std::vector<User>::iterator it = u_.begin();
            for( ; it != u_.end(); it++){
                SendMessage(message_, it->client, it->len);
            }
        }
        static void *ProductMessage(void *arg)
        {
            pthread_detach(pthread_self());
            ChatServer *serp_ = (ChatServer*)arg;
            std::string message_;
            for ( ; ; ){
                serp_->RecvMessage(message_);
            }
        }
        static void *ConsumeMessage(void *arg)
        {
            pthread_detach(pthread_self());
            ChatServer *serp_ = (ChatServer*)arg;
            for ( ; ; ){
                serp_->BroadcastMessage();
            }
        }
        static int HandlerLogout()
        {
            //同学们自己完成
        }
        static int HandlerLogin(int sock_, ChatServer *serp_, struct sockaddr_in &client_, socklen_t &len_)
        {
            UserManager &um_ = serp_->GetUserManager();
            struct LoginInfo lg_;
            recv(sock_, &lg_, sizeof(lg_), 0);
            return um_.Login(lg_.id, lg_.passwd, client_, len_);
        }
        static int HandlerRegister(int sock_, ChatServer *serp_, id_t &id_)
        {
            UserManager &um_ = serp_->GetUserManager();
            struct RegisterInfo rg_;
            recv(sock_, &rg_, sizeof(rg_), 0);
            id_ = um_.Register(rg_.nick_name, rg_.school, rg_.passwd);
            LOG(INFO, "new client register success!");
            return 0;
        }
        static void *HandlerNewClient(void *arg)
        {
            pthread_detach(pthread_self());
            LoginConnect *conn_ = (LoginConnect*)arg;
            int sock_ = conn_->sock;
            ChatServer *serp_ = (ChatServer*)conn_->server;
            id_t id_ = -1;
            int status_ = -1;
            char type_;
            if(recv(sock_, &type_, 1, 0) <= 0){
                LOG(ERROR, "recv type error!");
                goto end;
            }
            switch(type_){
                case LOGIN:
                    status_ = HandlerLogin(sock_, serp_, conn_->client, conn_->len);
                    break;
                case REGISTER:
                    status_ = HandlerRegister(sock_, serp_, id_);
                    break;
                case LOGOUT:
                    //status_ = HandlerLogout(id_);
                default:
                    LOG(ERROR, "type error!");
                    goto end;
            }

end:
            Reply rp_ = {status_, id_};
            send(sock_, &rp_, sizeof(rp_), 0);
            delete conn_;
            close(sock_);
        }
        void Start()
        {
            pthread_t c_,p_;
            pthread_create(&p_, NULL, ProductMessage, (void *)this);
            pthread_create(&c_, NULL, ConsumeMessage, (void *)this);

            for( ; ; ){
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                int new_sock_ = accept(login_sock, (struct sockaddr*)&peer, &len);
                if(new_sock_ < 0){
                    LOG(ERROR, "accept new sock error!");
                    continue;
                }
                pthread_t tid_;
                LoginConnect *conn_ = new LoginConnect(new_sock_, this, peer, len);
                pthread_create(&tid_, NULL, HandlerNewClient, (void *)conn_);
            }
        }
        ~ChatServer()
        {
            close(sock);
            close(login_sock);
        }
};

#endif

