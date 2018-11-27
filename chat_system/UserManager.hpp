#ifndef __USERMANAGER_HPP__
#define __USERMANAGER_HPP__

#include <iostream>
#include <unordered_map>
#include <vector>
#include <pthread.h>
#include "ProtocolUtil.hpp"
#include "Log.hpp"

#define USER_OFFLINE 0
#define USER_LOGINED 1
#define USER_ONLINE  2

class User{
    public:
        std::string nick_name;
        std::string school;
        id_type id;
        std::string passwd;
        struct sockaddr_in client;
        socklen_t len;
        int status;
    public:
        User(std::string &n_, std::string &s_, id_type &id_, std::string &passwd_)
        {
            nick_name = n_;
            school = s_;
            id = id_;
            passwd = passwd_;
            status = USER_OFFLINE;
        }
};

class UserManager{
    private:
        int id_count;
        int logined_count;
        std::unordered_map<id_type, User> register_user;
        //std::vector<User> logined_user;
        std::vector<User> online_user;
        pthread_mutex_t lock;
    private:
        void LockManager()
        {
            pthread_mutex_lock(&lock);
        }
        void UnLockManager()
        {
            pthread_mutex_unlock(&lock);
        }
        void GetId(id_type &id_)
        {
            id_ = id_count++;
        }
    public:
        UserManager()
        {
            id_count = 0;
            logined_count = 0;
            pthread_mutex_init(&lock, NULL);
        }
        bool SafeCheck(id_type id_, struct sockaddr_in &client_, socklen_t &len_)
        {
            std::unordered_map<id_type, User>::iterator it;
            it = register_user.find(id_);
            if(it == register_user.end()){
                return false;
            }
            if((it->second).status == USER_OFFLINE){
                return false;
            }
            if((it->second).status == USER_ONLINE){
                return true;
            }
            if((it->second).status == USER_LOGINED){
                (it->second).status = USER_ONLINE;
                (it->second).client = client_;
                (it->second).len = len_;
                online_user.push_back(it->second);
            }
            return true;
        }
        std::vector<User>& GetOnlineUser()
        {
            return online_user;
        }
        int Login(id_type id_, std::string passwd_, struct sockaddr_in &client_, socklen_t &len_)
        {
            int status;
            std::unordered_map<id_type, User>::iterator it;
            LockManager();
            it = register_user.find(id_);
            if(it != register_user.end()){
                if((it->second).passwd == passwd_){
                    LOG(INFO, "user Login success, add to Logined user!");
                    (it->second).status = USER_LOGINED;
                    logined_count++;
                    status = 0;
                }
                else{
                    status = -1;
                }
            }
            else{
                status = -1;
            }
            UnLockManager();
            return status;
        }
        int Logout(id_type id_)
        {
            //同学们自己完成
        }
        id_type Register(std::string nick_name_, std::string school_, std::string passwd_)
        {
            id_type id_;
            LockManager();
            GetId(id_);
            User u(nick_name_, school_, id_, passwd_);
            register_user.insert(std::make_pair(id_, u));
            UnLockManager();
            return id_;
        }
        ~UserManager()
        {
            id_count = 0;
            pthread_mutex_destroy(&lock);
        }
};

#endif

