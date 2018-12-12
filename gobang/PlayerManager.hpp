#ifndef _PLAYER_MANAGER_
#define _PLAYER_MANAGER_

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <time.h>
#include "RoomManager.hpp"
#include "Log.hpp"

#define LEVEL_NUM 1000

typedef enum{
    OFFLINE,
    ONLINE,
    MATCHING,
    PLAYING,
}status_t;

class Basic{
    public:
        int id;
        std::string passwd;
        std::string nick_name;
    public:
        Basic(int &id_, std::string &passwd_, std::string &nick_name_):id(0),passwd(passwd_),nick_name(nick_name_)
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
        status_t stat;
        char chessman;
        int room;
    public:
        Status():stat(OFFLINE),chessman(RED),room(-1)
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
        Player(int id_ = -1, std::string passwd_ ="", std::string nick_name_ = ""):\
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
            status.chessman = color_;
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
            return score.level;
        }
        status_t Stat()
        {
            return status.stat;
        }
        int Room()
        {
            return status.room;
        }
        int SetRoom(int room_number_)
        {
            status.room = room_number_;
        }
        int Wait()
        {
            struct timespec timeout;
            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_sec += WAIT_TIME;
            return pthread_cond_timedwait(&cond, &lock, &timeout);
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
    private:
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
            pthread_mutex_lock(&players_lock);
        }
        void UnlockPlayers()
        {
            pthread_mutex_unlock(&players_lock);
        }
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
        void GamePrepare(int room_id_, int id1_, int id2_)
        {
            PopMatchingPoolCore(id1_);
            PopMatchingPoolCore(id2_);

            players[id1_].SetRoom(room_id_);
            players[id2_].SetRoom(room_id_);
            players[id2_].SetChessColor('O');

            players[id1_].Playing();
            players[id2_].Playing();

            players[id1_].Wakeup();
            players[id2_].Wakeup();
        }
    public:
        PlayerManager():assign_id(0),match_pool(LEVEL_NUM),matching_players(0)
        {
            pthread_mutex_init(&players_lock, NULL);
            pthread_mutex_init(&pool_lock, NULL);
        }
        int Register(std::string nick_name_, std::string passwd_)
        {
            LockPlayers();
            int id_ = assign_id++;
            Player p_(id_, passwd_, nick_name_);
            players.insert({id_, p_});
            UnlockPlayers();
            LOG(INFO, "New Player Register");
            return id_;
        }
        bool Login( int id_, std::string passwd_ )
        {
            bool ret = false;
            if(IsPlayerLegal(id_, passwd_)){
                ret = true;
                Online(id_);
                LOG(INFO, "Player Login Success");
            }
            return ret;
        }
        bool Logout( int id_ )
        {
            LOG(INFO, "Player Logout Success");
            return Offline(id_);
        }
        bool Match(int id_)
        {
            bool ret = false;
            if(PushMatchPool(id_)){
                LOG(INFO, "Player Match Begin!");
                ret = PlayerWait(id_);
                LOG(INFO, "Player Match End!");
                PopMatchingPool(id_);
            }
            return ret;
        }
        char PlayerChessColor(int id_)
        {
            return players[id_].ChessColor();
        }
        Room GetRoom(int id_)
        {
            int room_id_ = players[id_].Room();
            return rm.GetRoom(room_id_);
        }
        int Game(int id_, int x_, int y_)
        {
            int room_id_ = players[id_].Room();
            char color_ = players[id_].ChessColor();
            int result_ = rm.Game(room_id_, id_, x_, y_, color_);
            return result_;
        }
        void GameEnd(int id_)
        {
            int room_id_ = players[id_].Room();
            players[id_].Online();
            rm.GameEnd(room_id_, id_);
            rm.DestroyRoom(room_id_);
        }
        void MatchService()
        {
            bool ret = false;
            std::vector<int> id_list_;

            LockMatchPool();
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
        ~PlayerManager()
        {
            pthread_mutex_destroy(&players_lock);
            pthread_mutex_destroy(&pool_lock);
        }
};
#endif
















