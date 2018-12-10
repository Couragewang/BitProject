#ifndef _PLAYER_MANAGER_
#define _PLAYER_MANAGER_

#include <iostream>
#include <string>
#include <unordered_map>
#include "RoomManager.hpp"

#define LEVEL_NUM 1000

class Basic{
    public:
        int id;
        std::string passwd;
        std::string nick_name;
    public:
        Basic(int &id_, std::string &passwd_, std::string &nick_name_):id(0),passwd(passwd_),nick_name(nick_name_);
        {}
};
class Score{
    public:
        int wins; // 1
        int loses;//-1
        int ties; // 0
        int level;
    public:
        Score():wins(0), loses(0), ties(0), level(0)
        {}
};
class Status{
    public:
        enum{
            OFFLINE,
            ONLINE,
            MATCHING,
            PLAYING,
        }stat;
        enum{
            RED='X',
            BLACK='O';
        }chessman;
        int room;
    public:
        Status():status(OFFLINE),chessman('X'),room(-1)
        {}
};

class Player{
    private:
        Basic basic;
        Score score;
        Status status;

        pthread_mutex_t lock;
        pthread_cond_t cond;
    public:
        Player(int id_, std::string passwd_, std::string nick_name_):\
            basic(id_, passwd_, nick_name_)
        {
            pthread_mutex_init(&lock, NULL);
            pthread_cond_init(&cond, NULL);
        }
        char ChessColor()
        {
            return status.chessman;
        }
        void SetChessColor(char color_)
        {
            return status.chessman = color_;
        }
        void Online()
        {
            status.stat = ONLINE;
        }
        void Offline()
        {
            status.stat = OFFLINE;
        }
        void Matching()
        {
            status.stat = MATCHING;
        }
        void Playing()
        {
            status.stat = PLAYING;
        }
        std::string Passwd()
        {
            return basic.passwd;
        }
        int Id()
        {
            return basic.id;
        }
        int Level()
        {
            return basic.level;
        }
        status_t Stat()
        {
            return basic.stat;
        }
        int Room()
        {
            return status.room;
        }
        int SetRoom(int room_number_)
        {
            status.room = room_number_;
        }
        void Wait()
        {
            pthread_cond_wait(&cond, &lock);
        }
        void Wakeup()
        {
            pthread_cond_signal(&cond);
        }
        ~Player()
        {
            pthread_mutex_destroy(&lock);
            pthread_cond_destroy(&cond);
        }
};

class PlayerManager{
    private:
        std::unordered_map<int,Player> players;
        int assign_id;
        pthread_mutex_t players_lock;

        std::vector< std::vector<int> > match_pool;
        int matching_players;
        pthread_mutex_t pool_lock;

        RoomManager rm;
    public:
        void LockMatchPool()
        {
            pthread_mutex_lock(&pool_lock);
        }
        void UnlockMatchPool()
        {
            pthread_mutex_unlock(&pool_lock);
        }
        void LockPlayers()
        {
            pthread_mutex_lock(&player_lock);
        }
        void UnlockPlayers()
        {
            pthread_mutex_unlock(&player_lock);
        }
        bool PopMatchingPoolCore(int id_)
        {
            bool ret = false;
            int level_ = players[id_].Level();
            std::vector<int> &v = match_pool[level_];
            std::vector<int>::iterator it = std::find(v.begin(), v.end(), player_);
            if(it != v.end()){
                v.erase(it);
                matching_players--;
                ret = true;
            }
            return false;
        }
    public:
        PlayerManager():assign_id(0),match_pool(LEVEL_NUM),matching_players(0)
        {
            pthread_mutex_init(&players_lock, NULL);
            pthread_mutex_init(&pool_lock, NULL);
        }
        int InsertNewPlayer(std::string &nick_name_, std::string passwd_)
        {
            LockPlayers();

            int id_ = assign_id++;
            Player p_(id_, passwd_, nick_name_);
            players.insert({id_, player_});

            UnlockPlayers();
            return id_;
        }
        bool IsPlayerLegal(int id_, std::string &passwd_)
        {
            bool ret = false;
            LockPlayers();

            auto search = players.find(id_);
            if(search != players.end() && passwd_ == (search->second).Passwd()){
                ret = true;
            }

            UnlockPlayers();

            return ret;
        }
        bool Online(int id_)
        {
            LockPlayers();
            players[id_].Online();
            UnlockPlayers();
        }
        bool Offline(int id_)
        {
            LockPlayers();
            players[id_].Offline();
            UnlockPlayers();
        }
        ///////////////
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

            UnlockMatchPool();

            return ret;
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
        bool PlayerWait(int id_)
        {
            while(players[id_].Stat() != Playing || players[id_].Room() == -1){
                players[id_].Wait();
            }
            return true;
        }
        void GamePrepare(int room_id_, int id1_, int id2_)
        {
            PopMatchingPoolCore(id1_);
            PopMatchingPoolCore(id2_);

            players[id1_].SetRoom(room_id);
            players[id2_].SetRoom(room_id);
            players[id2_].SetChessColor('O');

            Players[id1_].Playing();
            Players[id2_].Playing();

            Players[id1_].Wakeup();
            Players[id2_].Wakeup();
        }
        void Match()
        {
            bool ret = false;
            std::vector<int> id_list_;

            LockMatchPool();
            for(auto i = LEVEL_NUM; i >= 0; i--){
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
                    int room_id_ = pm.CreateRoom(v[j], v[j+1]);
                    GamePrepare(room_id, v[j], v[j+1]);
                }
            }
            int size_ = id_list_.size();
            size_ &= (~1);
            for (auto i=0; i < size_; i+=2) {
                int room_id_ = pm.CreateRoom(id_list_[i], id_list_[i+1]);
                GamePrepare(room_id, id_list_[i], id_list[i+1]);
            }
            UnlockMatchPool();
        }
        int Play(int id_, int x_, int y_)
        {
            int room_id_ = players[id_].Room();
            char color_ = players[id_].ChessColor();
            int result_ = rm.RoomGame(room_id_, id_, x_, y_, color_);
            if(result_ == -2 || result_ > 0){
                GameEnd();
            }
            return result;
        }
        ~PlayerManager()
        {
            pthread_mutex_destroy(&players_lock);
            pthread_mutex_destroy(&pool_lock);
        }
};
#endif
















