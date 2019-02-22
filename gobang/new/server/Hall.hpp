#pragma once

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

//        std::unordered_map<int,pthread_cond_t> wait_queue;
        //基本函数
        void LockPlayers()
        {
            pthread_mutex_lock(&players_lock);
        }
        void UnlockPlayers()
        {
            pthread_mutex_unlock(&players_lock);
        }
        bool IsPlayerLegal(const int &id_, const std::string &passwd_)
        {
            bool ret = false;
            auto search = players.find(id_);
            if(search != players.end() && passwd_ == (search->second).Passwd()){
                ret = true;
            }
            return ret;
        }
    public:
        PlayerManager():player_assign_id(10000)
        {
            pthread_mutex_init(&players_lock, NULL);
        }
        //用户基本管理，登录，注册，注销
        int Register(const std::string nick_name_, const std::string passwd_)
        {
            LockPlayers();
            int id_ = player_assign_id++;
            Player p_(id_, passwd_, nick_name_);
            players.insert({id_, p_});
            UnlockPlayers();
            return id_;
        }
        bool Login(const int &id_, const std::string &passwd_)
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
        bool Logout( const int &id_, const std::string &passwd_)
        {
            int ret = false;
            LockPlayers();
            if(IsPlayerLegal(id_, passwd_)){
                ret = true;
                players[id_].Offline();
            }
            UnlockPlayers();
            return ret;
        }
        bool PlayerMatching( int id_ )
        {
            LockPlayers();
            players[id_].Matching();
            UnlockPlayers();
            return true;
        }
        int PlayerRateOfWin(int id_)
        {
            return players[id_].RateOfWin();
        }
        void PlayerPrepare(int room_id, int p0, int p1)
        {
            LockPlayers();
            players[p0].Playing();
            players[p1].Playing();
            players[p0].SetRoom(room_id);
            players[p1].SetRoom(room_id);

            players[p0].SetChessColor(BLACK);
            players[p1].SetChessColor(WHITE);

            UnlockPlayers();
            players[p0].Wakeup();
            players[p1].Wakeup();
        }
        int PlayerWait(int id)
        {
            int room_id = -1;
            while(players[id].Stat() != PLAYING || players[id].Room() == -1){
                std::cout << "Play wait ..." << std::endl;
                if(players[id].Wait() == ETIMEDOUT){
                    room_id = -1;
                    break;
                }
                else{
                    room_id = players[id].Room();
                }
            }
            return room_id;
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
            int room_id_ = room_assign_id++;
            Room room_(id1_, id2_);
            rooms.insert({room_id_, room_});
            UnlockRooms();
            return room_id_;
        }
        void Move(const int &id, const int &room_id, const int &x, const int &y)
        {
            LockRooms();
            Room &room = rooms[room_id];
            if(room.IsSafe(id, x, y)){
                room.PlayChess(x, y);
                room.ChagePlayer();
                std::cout << "User is safe: " << id << std::endl;
            }else{
                std::cout << "User is Unsafe: " << id << std::endl;
            }
            UnlockRooms();
        }
        std::string GetRoomBoard(const int &room_id)
        {
            std::string board;
            LockRooms();
            if(rooms.find(room_id) != rooms.end()){
                board = (rooms[room_id]).GetBoard();
            }
            UnlockRooms();
            return board;
        }
        bool WhoPlay(const int &id, const int &room_id)
        {
            bool result = false;
            LockRooms();
            if(rooms.find(room_id) != rooms.end()){
                result = (rooms[room_id]).IsCurrRight(id);
            }
            UnlockRooms();
            return result;
        }
        int Judge(const int &room_id)
        {
            int result = 0;
            LockRooms();
            if(rooms.find(room_id) != rooms.end()){
                result = (rooms[room_id]).WhoWin();
            }
            UnlockRooms();
            return result;
        }
        void DestroyRoom(int room_id_)
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

    public:
        MatchManager():match_pool(MATCH_POOL_NUM)
        {
            matching_players = 0;
            pthread_mutex_init(&match_lock, NULL);
            pthread_cond_init(&match_cond, NULL);
        }
        void LockMatchPool()
        {
            pthread_mutex_lock(&match_lock);
        }
        void UnlockMatchPool()
        {
            pthread_mutex_unlock(&match_lock);
        }
        //将用户放入匹配池中
        bool PushMatchPool(int id_, int rate_)
        {
            bool ret = false;
            auto &v = match_pool[rate_];
            std::vector<int>::iterator it = std::find(v.begin(), v.end(), id_);
            if(it == v.end()){
                v.push_back(id_);
                matching_players++;
                ret = true;
            }
            return ret;
        }
        void CheckMatchPool()
        {
            while(matching_players <= 1){
                std::cout << "matching player is : " << matching_players << std::endl;
                pthread_cond_wait(&match_cond, &match_lock);
            }
            std::cout << "2 lastest players match..." << std::endl;
        }
        void Match(PlayerManager &pm, RoomManager &rm)
        {
            int size;
            std::vector<int> player_set;
            for(auto i = MATCH_POOL_NUM-1; i >= 0; i--){
                auto &v = match_pool[i];
                if(v.empty()){
                    continue;
                }
                size  = v.size();
                //将所有的用户压入匹配vector中
                for (auto j = 0; j < size; j++){
                    player_set.push_back(v[j]);
                }
            }
            //获取所有的匹配用户，保证是偶数个，有一定概率会让一个人匹配失败，可以考虑机器人补位
            size = (player_set.size() & (~1));
            for (auto i=0; i < size; i += 2) {
                int room_id = rm.CreateRoom(player_set[i], player_set[i+1]);
                pm.PlayerPrepare(room_id, player_set[i], player_set[i+1]);
            }
            std::cout << "Match Success..." << std::endl;
        }
        void Clear()
        {
            matching_players = 0;
            for(auto i = MATCH_POOL_NUM-1; i >= 0; i--){
                auto &v = match_pool[i];
                if(v.empty()){
                    continue;
                }
                std::vector<int>().swap(v);
            }
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
    public:
        PlayerManager pm;
        RoomManager rm;
        MatchManager mm;
        //客户要进行匹配请求
        int PlayerMatch(int id)
        {
            mm.LockMatchPool(); //锁住匹配池
            pm.PlayerMatching(id);
            int rate = pm.PlayerRateOfWin(id);
            mm.PushMatchPool(id, rate);
            mm.UnlockMatchPool();
            mm.MathThreadWakeup();
            return pm.PlayerWait(id);
        }
        //匹配线程周期性的要执行匹配任务
        void MatchService()
        {
            for(;;){
                mm.LockMatchPool(); //锁住匹配池
                mm.CheckMatchPool();
                mm.Match(pm, rm);
                mm.Clear();
                mm.UnlockMatchPool();
            }
        }
};
