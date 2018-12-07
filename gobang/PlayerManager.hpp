#ifndef _PLAYER_MANAGER_
#define _PLAYER_MANAGER_

#include <iostream>
#include <string>
#include <unordered_map>

#define LEVEL_NUM 1000
class Room;

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
        status_t Status()
        {
            return basic.stat;
        }
        int Room()
        {
            return status.room;
        }
        int SetRoom(int room_number_)
        {
            room = room_number_;
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
        pthread_mutex_t pool_lock;
    public:
        void LockMatchPool()
        {
            pthread_mutex_lock(&pool_lock);
        }
        void UnlockMatchPool()
        {
            pthread_mutex_lock(&pool_lock);
        }
    public:
        PlayerManager():assign_id(0),match_pool(LEVEL_NUM);
        {
            pthread_mutex_init(&players_lock, NULL);
            pthread_mutex_init(&pool_lock, NULL);
        }
        bool PushMatchPool(int id_)
        {
            bool ret = false;
            int level_ = players[id_].Level();
            std::vector<int> &v = match_pool[level_];

            LockMatchPool();

            std::vector<int>::iterator it = std::find(v.begin(), v.end(), id_);
            if(it == v.end()){
                v.push_back(id_);
                ret = true;
            }

            UnlockMatchPool();

            return ret;
        }
        void PopMatchingPool(int id_)
        {
            int level_ = players[id_].Level();
            std::vector<int> &v = match_pool[level_];
            std::vector<int>::iterator it = std::find(v.begin(), v.end(), player_);
            if(it != v.end()){
                v.erase(it);
            }
        }
        bool InMatchingPool(int id_)
        {
            bool ret = false;
            pthread_mutex_lock(&player_lock);
            std::unordered_map<int, Player>::iterator it = players.find(id_);
            if(it != players.end()){
                Player &player_ = it->second;
                pthread_mutex_unlock(&player_lock);
                player_.Matching();
                ret = AddPlayerToMatchingPool(player_);
            }
            else{
                pthread_mutex_unlock(&player_lock);
            }
            return ret;
        }
        bool PlayerWait(int id_)
        {
            pthread_mutex_lock(&player_lock);
            std::unordered_map<int, Player>::iterator it = players.find(id_);
            if(it != players.end()){
                Player &player_ = it->second;
                pthread_mutex_unlock(&player_lock);
                player_.Wait();
                return player_.Status() == PLAYING;
            }
            else{
                pthread_mutex_lock(&player_lock);
            }
            return false;
        }
        void PlayPrepare(Player *player_one_, Player *player_two_)
        {
            room_manager.CreateNewRoom(player_one_, player_two_);
        }
        bool MatchBegin()
        {
            for(auto i_ = LEVEL_NUM - 1; i_ >= 0; i_--){
                pthread_mutex_lock(&match_lock);

                Player *player_alone = NULL;
                std::vector<Player*> &v = match_players[i_];
                if(!v.empty()){
                    if(v.size() & 1){
                        if(player_alone == NULL){
                            player_alone = v[v.size()-1];
                        }
                        else{
                            Player *player_one_ = player_alone;
                            Player *player_two_ = v[size_-1];
                            player_alone = NULL;
                            PlayPrepare(player_one_, player_two_);
                            RemovePlayerFromMatchingPool(player_one_);
                            RemovePlayerFromMatchingPool(player_two_);
                            player_one_.Playing();
                            player_two_.Playing();
                            player_one_.Wakeup();
                            player_two_.Wakeup();
                        }
                    }
                    for(auto j_ = 0; j_ < v.size()-1; j_+=2){
                        Player *play_one_ = v[j_];
                        Player *play_two_ = v[j_+1];
                        PlayPrepare(player_one_, player_two_);
                        RemovePlayerFromMatchingPool(player_one_);
                        RemovePlayerFromMatchingPool(player_two_);
                        player_one_.Playing();
                        player_two_.Playing();
                        player_one_.Wakeup();
                        player_two_.Wakeup();
                    }
                }

                pthread_mutex_unlock(&match_lock);
            }
        }
        ~PlayerManager()
        {
            pthread_mutex_destroy(&players_lock);
            pthread_mutex_destroy(&pool_lock);
        }
};
#endif
















