#ifndef _HALL_HPP_
#define _HALL_HPP_

#include <iostream>
#include <pthread.h>
#include <unordered_map>
#include <unistd.h>
#include "PlayerManager.hpp"
#include "RoomManager.hpp"
#include "Protocol.hpp"
#include "ThreadPool.hpp"

#define STEP 1024
#define SERVER_PORT 8888
#define LEVEL_NUM 1000

class Hall{
private:
    //用户管理
    std::unordered_map<int,Player> players;
    int player_assign_id;
    pthread_mutex_t players_lock;
    //匹配管理
    std::vector< std::vector<int> > match_pool;
    int matching_players;
    pthread_mutex_t pool_lock;
    pthread_cond_t pool_cond;
    //房间管理
    int room_assign_id;
    pthread_mutex_t room_lock;
        
    bool IsPlayerLegal(int id_, std::string &passwd_)
    {
        bool ret = false;
        auto search = players.find(id_);
        if(search != players.end() && passwd_ == (search->second).Passwd()){
            ret = true;
        }
        return ret;
    }
    public:
        Hall():player_assign_id(0), room_assign_id(0)
        {
            matching_players = 0;
            pthread_mutex_init(&players_lock, NULL);
            pthread_mutex_init(&pool_lock, NULL);
            pthread_cond_init(&pool_cond, NULL);
            pthread_mutex_init(&room_lock, NULL);
        }
        void LockPlayers()
        {
            pthread_mutex_lock(&players_lock);
        }
        void UnlockPlayers()
        {
            pthread_mutex_unlock(&players_lock);
        }
        void LockMatchPool()
        {
            pthread_mutex_lock(&pool_lock);
        }
        void UnlockMatchPool()
        {
            pthread_mutex_unlock(&pool_lock);
        }
        void MathServiceWakeup()
        {
            pthread_cond_signal(&pool_cond);
        }
        int Register(std::string nick_name_, std::string passwd_)
        {
            LockPlayers();
            int id_ = player_assign_id++;
            Player p_(id_, passwd_, nick_name_);
            players.insert({id_, p_});
            UnlockPlayers();
            return id_;
        }
        bool Login(int id_, std::string passwd_)
        {
            bool ret = false;
            LockPlayers();
            if(IsPlayerLegal(id_, passwd_)){
                ret = true;
                players[id_].Online();
            }
            UnlockPlayers();
            return ret;
        }
        bool Logout( int id_, std::string passwd_ )
        {
            bool ret = false;
            LockPlayers();
            if(IsPlayerLegal(id_, passwd_)){
                ret = true;
                players[id_].Offline();
            }
            UnlockPlayers();
            return ret;
        }
        int PlayerWait(int id)
        {
            bool ret = false;
            int code = 0;
            while(players[id_].Stat() != PLAYING || players[id_].Room() == -1){
                if(players[id_].Wait() == ETIMEDOUT){
                    break;
                }
                else{
                    ret = true;
                }
            }
            return ret;
        }
        bool PushMatchPool(int id_)
        {
            bool ret = false;
            players[id_].Matching();

            LockMatchPool();

            int level_ = players[id_].Level();
            std::vector<int> &v = match_pool[level_];
            std::vector<int>::iterator it = std::find(v.begin(), v.end(), id_);
            if(it == v.end()){
                v.push_back(id_);
                matching_players++;
                ret = true;
            }
            MathServiceWakeup();
            UnlockMatchPool();

            return ret;
        }
        void MathServiceWait()
        {
            pthread_cond_wait(&pool_cond, &pool_lock);
        }
        void MatchService()
        {
            bool ret = false;
            std::vector<int> id_list_;
            LockMatchPool();
            while(matching_players < 2){
                MathServiceWait();
            }
            for(auto i = LEVEL_NUM-1; i >= 0; i--){
                auto &v = match_pool[i];
                if(v.empty()){
                    continue;
                }
                int size_ = v.size();
                if(size_ & 1){
                    id_list_.push_back(v[size_-1]);
                    size_--;
                }
                for(auto j = 0; j < size_; j+=2){
                    int room_id_ = rm.CreateRoom(v[j], v[j+1]);
                    GamePrepare(room_id_, v[j], v[j+1]);
                }
            }
            int size_ = id_list_.size();
            size_ &= (~1);
            for (auto i=0; i < size_; i+=2) {
                int room_id_ = rm.CreateRoom(id_list_[i], id_list_[i+1]);
                GamePrepare(room_id_, id_list_[i], id_list_[i+1]);
            }
            UnlockMatchPool();
        }
        ~Hall()
        {
            pthread_mutex_destroy(&players_lock);
            pthread_mutex_destroy(&pool_lock);
            pthread_cond_destroy(&pool_cond);
            pthread_mutex_destroy(&room_lock);
        }
};

class Singleton{
    private:
        static Hall *hp;
        static pthread_mutex_t single_lock;
    private:
        Singleton()
        {}
        Singleton(const Singleton &)
        {}
    public:
        static Hall* GetInstance()
        {
            if( hp == NULL ){
                pthread_mutex_lock(&single_lock);
                if( hp == NULL ){
                    hp = new Hall();
                }
                pthread_mutex_unlock(&single_lock);
            }
            return hp;
        }
};
pthread_mutex_t Singleton::single_lock = PTHREAD_MUTEX_INITIALIZER;
Hall *Singleton::hp = NULL;

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
        static void HandlerEvent(int sock)
        {
            char cmd;
            recv(sock, &cmd, 1, 0);
            std::string msg;
            SocketApi::Recv(sock, msg);
            Hall *p = Singleton::GetInstance();
            switch(cmd){
                case 'r'://register
                    {
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
        static void HandlerMatch(int no_arg)
        {
            Hall *p = Singleton::GetInstance();

        }
        void Start()
        {
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

