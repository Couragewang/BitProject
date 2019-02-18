#ifndef __HALL_HPP__
#define __HALL_HPP__

#include <iostream>
#include "Player.hpp"
#include "Room.hpp"

#define STEP 1024
#define SERVER_PORT 8888
//#define LEVEL_NUM 1000
#define MATCH_POOL_NUM 101

class MatchManager;
class PlayerManager;
class RoomManager;

class PlayerManager{
    private:
        //用户管理
        std::unordered_map<int,Player> players;
        int player_assign_id;
        pthread_mutex_t players_lock;
        std::unordered_map<int,pthread_cond_t> wait_queue;
        //基本函数
        void LockPlayers()
        {
            pthread_mutex_lock(&players_lock);
        }
        void UnlockPlayers()
        {
            pthread_mutex_unlock(&players_lock);
        }
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
        PlayerManager():player_assign_id(0)
        {
            pthread_mutex_init(&players_lock, NULL);
        }
        //用户基本管理，登录，注册，注销
        int InsertPlayer(std::string nick_name_, std::string passwd_)
        {
            LockPlayers();
            int id_ = player_assign_id++;
            Player p_(id_, passwd_, nick_name_);
            players.insert({id_, p_});
            UnlockPlayers();
            return id_;
        }
        bool PlayerOnline(int id_, std::string passwd_)
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
        bool PlayerOffline( int id_, std::string passwd_ )
        {
            LockPlayers();
            players[id_].Offline();
            UnlockPlayers();
            return true;
        }
        void PlayerPrepare(int room_id, int p0, int p1)
        {
            LockPlayers();
            players[p0].Playing();
            players[p1].Playing();
            players[p0].SetRoom(room_id_);
            players[p1].SetRoom(room_id_);

            players[p0].SetChessColor(BLACK);
            players[p1].SetChessColor(WHITE);

            UnlockPlayers();
            players[p0].Wakeup();
            players[p1].Wakeup();
        }
        void PushWaitQueue(int id)
        {
            LockPlayers();
            pthread_cond_t cond;
            pthread_cond_init(&cond, NULL);
            wait_queue.insert({id, cond});
            LockPlayers();
        }
        void PopWaitQueue(int id)
        {
            LockPlayers();
            if(wait_queue.find(id) != rooms.end()){
                rooms.erase(room_id_);
            }
            LockPlayers();
        }
        void PlayerWait(int id)
        {
            bool ret = false;
            while(players[id].Stat() != PLAYING || players[id].Room() == -1){
                if(players[id_].Wait() == ETIMEDOUT){
                    break;
                }
                else{
                    ret = true;
                }
            }
        }
        ~PlayerManager()
        {
            pthread_mutex_destroy(&players_lock);
        }
};

class RoomManager{
    private:
        //房间管理
        std::unordered_map<int,Room> rooms;
        int room_assign_id;
        pthread_mutex_t room_lock;

        void LockRooms()
        {
            pthread_mutex_lock(&room_lock);
        }
        void UnlockRooms()
        {
            pthread_mutex_unlock(&room_lock);
        }
    public:
        RoomManager():room_assign_id(0)
        {
            pthread_mutex_init(&room_lock, NULL);
        }
        int CreateRoom(int id1_, int id2_)
        {
            LockRooms();
            int room_id_ = assign_id++;
            Room room_(id1_, id2_);
            rooms.insert({room_id_, room_});
            UnlockRooms();
            return room_id_;
        }
        int DestroyRoom(int room_id_)
        {
            LockRooms();
            if(rooms.find(room_id_) != rooms.end()){
                rooms.erase(room_id_);
            }
            UnlockRooms();
        }
        ~RoomManager()
        {
            pthread_mutex_destroy(&room_lock);
        }
};

class MatchManager{
    private:
        //匹配管理
        std::vector< std::vector<int> > match_pool;
        int matching_players;
        pthread_mutex_t match_lock;
        pthread_cond_t match_cond;

        void LockMatchPool()
        {
            pthread_mutex_lock(&match_lock);
        }
        void UnlockMatchPool()
        {
            pthread_mutex_unlock(&match_lock);
        }
    public:
        MatchManager():match_pool(MATCH_POOL_NUM)
        {
            matching_players = 0;
            pthread_mutex_init(&match_lock, NULL);
            pthread_cond_init(&match_cond, NULL);
        }
        //将用户放入匹配池中
        bool PushMatchPool(int id_)
        {
            bool ret = false;
            players[id_].Matching();

            int Rate_ = players[id_].RateOfWin();
            auto &v = match_pool[level_];
            std::vector<int>::iterator it = std::find(v.begin(), v.end(), id_);
            if(it == v.end()){
                v.push_back(id_);
                matching_players++;
                ret = true;
            }
            MathThreadWakeup();
            return ret;
        }
        bool IsNeedWait()
        {
            while(matching_players < 1){
                pthread_cond_wait(&match_cond, &match_lock);
            }
        }
        void Match(PlayerManager &pm, RoomManager &rm)
        {
            bool ret = false;
            std::vector<int> player_set;
            for(auto i = MATCH_POOL_NUM-1; i >= 0; i--){
                auto &v = match_pool[i];
                if(v.empty()){
                    continue;
                }
                //将所有的用户压入匹配vector中
                for (auto j = 0; j < v.size(); j++){
                    player_set.push_back(v[j]);
                }
            }
            //获取所有的匹配用户，保证是偶数个，有一定概率会让一个人匹配失败，可以考虑机器人补位
            int size = (player_set.size() & (~1));
            for (auto i=0; i < size; i += 2) {
                int room_id = rm.CreateRoom(player_set[i], player_set[i+1]);
                pm.PlayerPrepare(room_id, player_set[i], player_set[i+1]);
            }
        }
        void Clear()
        {
            for(auto i = MATCH_POOL_NUM-1; i >= 0; i--){
                auto &v = match_pool[i];
                if(v.empty()){
                    continue;
                }
                std::vector<int>().swap(v);
            }
        }
        /////////////////////////////////////////////////////////
        bool PopMatchingPoolCore(int id_)
        {
            bool ret = false;
            int level_ = players[id_].Level();
            std::vector<int> &v = match_pool[level_];
            std::vector<int>::iterator it = std::find(v.begin(), v.end(), id_);
            if(it != v.end()){
                v.erase(it);
                matching_players--;
                ret = true;
            }
            return false;
        }
        bool PopMatchingPool(int id_)
        {
            bool ret = false;
            LockMatchPool();
            ret = PopMatchingPoolCore(id_);
            UnlockMatchPool();
            players[id_].Playing();
            return ret;
        }
        void MathThreadWakeup()
        {
            pthread_cond_signal(&match_cond);
        }
        ~MatchManager()
        {
            pthread_mutex_destroy(&match_lock);
            pthread_cond_destroy(&match_cond);
        }
};

class Hall{
    private:
        PlayerManager pm;
        RoomManager rm;
        MatchManager mm;
    public:
        int Register(std::string nick_name_, std::string passwd_)
        {
            return pm.InsertPlayer(nick_name_,passwd_);
        }
        bool Login(int id_, std::string passwd_)
        {
            return pm.PlayerOnline(id_, passwd_);
        }
        bool Logout( int id_, std::string passwd_)
        {
            return pm.PlayerOffline(id_, passwd_);
        }
        //客户要进行匹配请求
        bool PlayerMatch(int id)
        {
            pm.PlayerWaitSet(id);
            mm.LockMatchPool(); //锁住匹配池
            mm.PushMatchPool(id);
            mm.UnlockMatchPool();
            return pm.PlayerWait(id);
        }
        //匹配线程周期性的要执行匹配任务
        void MatchTask()
        {
            for(;;){
                mm.LockMatchPool(); //锁住匹配池
                mm.IsNeedWait();
                mm.Match(pm, rm);
                mm.Clear();
                mm.UnlockMatchPool();
            }
        }
};

//将hall设计为单例模式，供server使用
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

#endif





