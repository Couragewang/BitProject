#ifndef __USERMANAGER_HPP__
#define __USERMANAGER_HPP__

#include <iostream>
#include <unordered_map>
#include <vector>
#include <pthread.h>
#include "ProtocolUtil.hpp"

class User{
    public:
        std::string nick_name;
        std::string school;
        id_type id;
        std::string passwd;
    public:
        User(std::string &n_, std::string &s_, id_type &id_, std::string &passwd_)
        {
            nick_name = n_;
            school = s_;
            id = id_;
            passwd = passwd_;
        }
};

class UserManager{
    private:
        int id_count;
        int logined_count;
        std::unordered_map<id_type, User> register_user;
        std::vector<User> logined_user;
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
        std::vector<User>& GetLoginedUser()
        {
            return logined_user;
        }
        int Login(id_type id_, std::string passwd_)
        {
            int status;
            std::unordered_map<id_type, User>::iterator it;
            LockManager();
            it = register_user.find(id_);
            if(it != register_user.end()){
                if((it->second).passwd == passwd_){
                    logined_user.push_back((it->second));
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

