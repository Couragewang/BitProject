#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <time.h>

#define WHITE 'X' //master
#define BLACK 'O'
#define NO    '-' //新创建，没有棋子
#define WAIT_TIME 30

typedef enum{
    OFFLINE,
    ONLINE,
    MATCHING,
    PLAYING,
}status_t;

class Player{
    private:
        //基本信息
        int id;
        std::string passwd;
        std::string nick_name;
        //游戏信息
        int wins; // 1
        int loses;//-1
        int ties; // 0
        //状态信息
        status_t stat;
        char chessman;
        int room_id;
        bool is_master;

        pthread_mutex_t lock;
        pthread_cond_t cond;

    public:
        Player(int id_ = -1, std::string passwd_ ="", std::string nick_name_ = "")
            :id(id_),passwd(passwd_),nick_name(nick_name_)
        {
            wins = 0;
            loses = 0;
            ties = 0;
            stat = OFFLINE;
            chessman = NO;
            room_id = -1;
            is_master = false;

            pthread_mutex_init(&lock, NULL);
            pthread_cond_init(&cond, NULL);
        }
        void SetChessColor(char color_)
        {
            chessman = color_;
        }
        char ChessColor()
        {
            return chessman;
        }
        void Online()
        {
            stat = ONLINE;
        }
        void Offline()
        {
            stat = OFFLINE;
        }
        void Matching()
        {
            stat = MATCHING;
        }
        void Playing()
        {
            stat = PLAYING;
        }
        std::string Passwd()
        {
            return passwd;
        }
        int Id()
        {
            return id;
        }
        int RateOfWin()
        {
            if((wins + loses) == 0){
                return 0;
            }
            return (wins)/(wins + loses);
        }
        status_t Stat()
        {
            return stat;
        }
        int Room()
        {
            return room_id;
        }
        void SetRoom(int room_number_)
        {
            room_id = room_number_;
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

#endif
















