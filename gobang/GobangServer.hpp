#ifndef _GOBANG_HPP_
#define _GOBANG_HPP_

#include <iostream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <pthread.h>
#include "buttonrpc.hpp"

#define ROW 5
#define COL 5

#define LEVEL_NUM 1000
#define ROOM_NUM 5000

typedef enum{
    OFFLINE,
    ONLINE,
    MATCHING,
    PLAYING,
}status_t;

typedef enum{
    BLACK,
    RED,
    NONE,
}chessman_t;

#define DOGFALL    0
#define WIN 1
#define ROUND_NEXT 2

class Room;

class Player{
    private:
        int id;
        std::string passwd;
        status_t status;
        std::string nick_name;
        chessman_t chessman;
        int win_count; // 1
        int lose_count;//-1
        int tie_count; // 0

        int level; //3000
        Room *room;

        pthread_mutex_t lock;
        pthread_cond_t cond;
    public:
        Player(int id_, std::string passwd_, std::string nick_name_):\
            id(id_), passwd(passwd_), \
            status(OFFLINE), nick_name(nick_name_), \
            checcman(NONE), win_count(0), lose_count(0),\
            tie_count(0), level(0), room_number(-1)
        {
            pthread_mutex_init(&lock, NULL);
            pthread_cond_init(&cond, NULL);
        }
        char ChessColor()
        {
            if(chessman == BLACK){
                return 'X';
            }
            else if(chessman == RED){
                return 'O';
            }
            else{
                return '*';
            }
        }
        void Online()
        {
            status = ONLINE;
        }
        void Offline()
        {
            status = OFFLINE;
        }
        void Matching()
        {
            status = MATCHING;
        }
        void Playing()
        {
            status = PLAYING;
        }
        std::string Passwd()
        {
            return passwd;
        }
        int Id()
        {
            return id;
        }
        int Level()
        {
            return level;
        }
        status_t Status()
        {
            return status;
        }
        void SetPlayRoom(int room_)
        {
            room_number = room_;
        }
        int SetRoom(Room *room_)
        {
            room = room_;
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

class Room{
    private:
        char board[ROW][COL];
        int curr_id;
        Player *player_one;
        Player *player_two;
        int who_win;

        bool IsPosLegal(const int &x_, const int &y_)
        {
            return board[x_-1][y_-1] == ' ';
        }
        bool IsPlayerRight(Player &player_)
        {
            return curr_id == player_.GetId();
        }
        void SwitchPlayer(Player &player_)
        {
            if(curr_id == player_one.GetId()){
                curr_id = player_two.GetId();
            }
            else{
                curr_id = player_one.GetId();
            }
        }
        int Judge()
        {
            for(auto i_ = 0; i_ < ROW; i_++){
                if( (board[i_][0] == board[i_][1]) &&\
                    (board[i_][1] == board[i_][2]) &&\
                    (board[i_][2] == board[i_][3]) &&\
                    (board[i_][3] == board[i_][4]) ){
                    return WIN;
                }
                if( (board[0][i_] == board[1][i_]) &&\
                    (board[1][i_] == board[2][i_]) &&\
                    (board[2][i_] == board[3][i_]) &&\
                    (board[3][i_] == board[4][i_]) ){
                    return WIN;
                }
            }
            if( (board[0][0] == board[1][1]) &&\
                (board[1][1] == board[2][2]) &&\
                (board[2][2] == board[3][3]) &&\
                (board[3][3] == board[4][4]) ){
                return WIN;
            }
            if( (board[0][4] == board[1][3]) &&\
                (board[1][3] == board[2][2]) &&\
                (board[2][2] == board[3][1]) &&\
                (board[3][1] == board[4][0]) ){
                return WIN;
            }
            for(auto i_ = 0; i_ < ROW; i_++){
                for(auto j_ = 0; j_ < COL; j_++){
                    if(board[i_][j_] == ' '){
                        return ROUND_NEXT;
                    }
                }
            }
            return DOGFALL;
        }
    public:
        Room(Player *one_, Player *two_):
            player_one(one_), player_two(two_), curr_id(player_one.GetId()), who_win(-1)
        {
            memset(board, ' ', sizeof(char)*ROW*COL);
        }
        int Play(Player &player_, int x_, int y_)
        {
            if(!IsPlayerRight(player_)){
                return -1;
            }
            int result;
            if( x_ >= 1 && x <= 5 && y_ >= 1 && y_ <= 5){
                if(IsPosLegal(x_, y_)){
                    char color_ = player_.GetChessColor();
                    board[x_ - 1][y_ - 1] = color_;
                    switch(Judge()){
                        case DOGFALL:
                            result = -2;
                            break;
                        case WIN: //win
                            result = player_.GetId();
                            break;
                        case ROUND_NEXT: //normal
                            SwitchPlayer(player_);
                            result = -3;
                            break;
                        default:
                            break;
                    }
                }
                else{
                    result = -4;
                }
            }
            return result;
        }
        ~Room()
        {
        }
};

class RoomMamager{
    private:
        std::vector<Room> game_hall;
        pthread_mutex_t lock;
    public:
        RoomManager()
        {
            pthread_mutex_init(&lock, NULL);
        }
        int CreateNewRoom(Player *player_one_, Player *player_two_)
        {
            pthread_mutex_lock(&lock);
            Room room_(player_one_, player_two_);
            game_hall.push_back(room_);
            player_one_.SetRoom(&game_hall[game_hall.size()-1]);
            player_two_.SetRoom(&game_hall[game_hall.size()-1]);
            pthread_mutex_lock(&unlock);
            return 0;
        }
        int DestroyRoom(int id)
        {
            pthread_mutex_lock(&lock);
            game_hall.erase(game_hall.begin() + id);
            pthread_mutex_unlock(&lock);
        }
        ~RoomManager()
        {
            pthread_mutex_destroy(&lock);
        }
};

class PlayerManager{
    private:
        std::unordered_map<int,Player> players;
        int players_nums;
        pthread_mutex_t player_lock;

        std::vector< std::vector<Player*> > match_players;
        pthread_mutex_t match_lock;

        RoomMamager   room_manager;
    private:
        bool AddPlayerToMatchingPool(Player &player_)
        {
            bool ret = false;
            int &level_ = player_.GetLevel();
            std::vector<Player*> &v = match_players[level_];
            pthread_mutex_lock(&match_lock);

            std::vector<Player*>::iterator it = std::find(v.begin(), v.end(), &player_);
            if(it == v.end()){
                v.push_back(&player_);
                ret = true;
            }
            pthread_mutex_unlock(&match_lock);

            return ret;
        }
        void RemovePlayerFromMatchingPool(Player *player_)
        {
            int &level_ = player_->GetLevel();
            std::vector<Player*> &v = match_players[level_];
            std::vector<Player*>::iterator it = std::find(v.begin(), v.end(), player_);
            if(it != v.end()){
                v.erase(it);
            }
        }
    public:
        PlayerManager():player_nums(0),match_players(LEVEL_NUM);
        {
            pthread_mutex_init(&player_lock, NULL);
            pthread_mutex_init(&match_lock, NULL);
        }
        bool Register(std::string &nick_name_, std::string &passwd_)
        {
            pthread_mutex_lock(&player_lock);
            int id_ = players_nums++;
            Player player_(id_, passwd_, nick_name_);
            players.insert({id_, player_});
            pthread_mutex_unlock(&player_lock);
            return true;
        }
        bool Login( int id_, std::string passwd_ )
        {
            bool ret = false;
            pthread_mutex_lock(&player_lock);
            std::unordered_map<int, Player>::iterator it = players.find(id_);
            if(it != players.end()){
                Player &player_ = it->second;
                if(player_.Passwd() == passwd_){
                    //login success
                    player_.Online();
                    ret = true;
                }
            }
            pthread_mutex_unlock(&player_lock);

            return ret;
        }
        bool Logout( int id_ )
        {
            bool ret = false;
            pthread_mutex_lock(&player_lock);
            std::unordered_map<int, Player>::iterator it = players.find(id_);
            if(it != players.end()){
                Player &player_ = it->second;
                player_.Offline();
                ret = true;
            }
            pthread_mutex_unlock(&player_lock);

            return ret;
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
                            player_alone = v[size_-1];
                            size_--;
                        }
                        else{
                            Player *player_one_ = player_alone;
                            Player *player_two_ = v[size_-1];
                            size_--;
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
            pthread_mutex_destroy(&lock);
            pthread_mutex_destroy(&match_lock);
        }
};

class GameManager{
    private:
        PlayerManager player_manager;
        pid_t match_thread;
    public:
        GameManager():room_manager(ROOM_NUM)
        {
            pthread_mutex_init(&match_lock, NULL);
        }
        void MatchRun()
        {
            player_manager.MatchBegin();
        }
        static void *MatchService(void *arg)
        {
            pthread_detach(pthread_self());
            GameManager *gm = (GameManager*)arg;
            for( ; ; ){
                gm->MatchRun();
            }
        }
        void StartMatchThread()
        {
            pthread_create(&match_thread, NULL, MatchService, (void*)this);
        }
        bool PlayerMatch(int id_)
        {
            bool ret = player_manager.InMatchingPool(id_);
            if(ret){
                int ret = player_manager.PlayerWait(id_);
            }
            return ret;
        }
        int Game(int id_, int x_, int y_)
        {
            std::unordered_map<int, Player>::iterator it = players.find(id_);
            if(it != players.end()){
                Player &player_ = it->second;
                Room *room_ = player_.GetPlayRoom();
                int result = room_->Play(player_, x_, y_);
            }
        }
        ~GameManager()
        {
            pthread_mutex_destroy(&match_lock);
        }
};


#endif














