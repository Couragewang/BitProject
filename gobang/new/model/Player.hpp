#ifndef _PLAYER_MANAGER_
#define _PLAYER_MANAGER_

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <time.h>

#define LEVEL_NUM 1000
#define RED 'r'
#define BLACK 'b'

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
        Basic(int &id_, std::string &passwd_, std::string &nick_name_):id(id_),passwd(passwd_),nick_name(nick_name_)
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
        int room_id;
    public:
        Status():stat(OFFLINE),chessman(RED),room_id(-1)
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
        void SetChessColor(char color_)
        {
            status.chessman = color_;
        }
        char GetChessColor()
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
            return score.level;
        }
        status_t Stat()
        {
            return status.stat;
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
        int Room()
        {
            return status.room_id;
        }
        void SetRoom(int room_number_)
        {
            status.room_id = room_number_;
        }
        ~Player()
        {
            pthread_mutex_destroy(&lock);
            pthread_cond_destroy(&cond);
        }
};
#endif
















