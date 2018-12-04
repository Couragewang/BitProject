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

#define LEVEL_NUM 3000

typedef enum{
    OFFLINE,
    ONLINE,
    MATCHING,
    PLAYING,
}status_t;

typedef enum{
    BLANK,
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
        int wins; // 1
        int loses;//-1
        int ties; // 0

        int level; //3000
        Room *play_room;
    public:
        Player(const int id_, std::string passwd_, std::string nick_name_):\
            id(id_), passwd(passwd_), status(OFFLINE), nick_name(nick_name_), checcman(NONE),\
            wins(0), loses(0), ties(0), level(0)
        {}
        char GetChessColor()
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
        std::string &GetPasswd()
        {
            return passwd;
        }
        int GetId()
        {
            return id;
        }
        int GetLevel()
        {
            return level;
        }
        void SetStatus(status_t st_)
        {
            status = st_;
        }
        void SetPlayRoom(Room *r_)
        {
            play_room = r_;
        }
        Room *GetPlayRoom()
        {
            return play_room;
        }
        ~Player()
        {}
};

class Room{
    private:
        char board[ROW][COL];
        int curr_id;
        Player &player_one;
        Player &player_two;
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
        Room(Player &one_, Player &two_):
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

GameManager{
    private:
        std::unordered_map<int,Player> players; //
        int players_nums;
        pthread_mutex_t lock;
        std::vector< std::vector<Player*> > match_players;
        std::vector<Room*> game_hall;
    private:
        void RemoveFromMatching(Player &player_)
        {
            int &level_ = player_.GetLevel();
            std::vector<Player*> &v = match_players[level_];
            std::vector<Player*>::iterator it = std::find(v.begin(), v.end(), &player_);
            if(it != v.end()){
                v.erase(it);
            }
        }
        void OnlinePlayer(Player &player_)
        {
            player_.SetStatus(ONLINE);
        }
        void OfflinePlayer(Player &player_)
        {
            player_.SetStatus(OFFLINE);
            RemoveFromMatching(player_);
        }
        bool MatchingPlayer(Player &player_)
        {
            player_.SetStatus(MATCHING);
            int &level_ = player_.GetLevel();
            std::vector<Player*> &v = match_players[level_];
            std::vector<Player*>::iterator it = std::find(v.begin(), v.end(), &player_);
            if(it == v.end()){
                v.push_back(&player_);
            }
            return true;
        }
        void PlayingPlayer(Player &player_)
        {
            player_.SetStatus(PLAYING);
            RemoveFromMatching(player_);
        }
        bool MatchBegin(Player &player_)
        {
            int &level_ = player_.GetLevel();
            for(auto i = level_; i >= 0; i++){
                std::vector<Player*> &v = match_players[i_];
                if(!v.empty()){
                    Player &player_two_ = *(v[0]);
                    PlayingPlayer(player_);
                    PlayingPlayer(player_two_);

                    Room *room_ = new Room(player_, player_two_);

                    player_.SetPlayRoom(room_);
                    player_two_.SetPlayRoom(room_);

                    game_hall.push_back(room_);
                }
            }
        }
    public:
        GameManager():players_nums(0),match_players(LEVEL_NUM)
        {
            pthread_mutex_init(&lock, NULL);
        }
        bool Register(std::string &nick_name_, std::string &passwd_)
        {
            pthread_mutex_lock(&lock);
            int id_ = players_nums++;
            Player player_(id_, passwd_, nick_name_);
            players.insert({id_, player_});
            pthread_mutex_unlock(&lock);
            return true;
        }
        bool Login( int id_, std::string passwd_ )
        {
            std::unordered_map<int, Player>::iterator it = players.find(id_);
            if(it != players.end()){
                Player &player_ = it->second;
                if(player_.GetPasswd() == passwd_){
                    //login success
                    pthread_mutex_lock(&lock);
                    OnlinePlayer(player_);
                    pthread_mutex_unlock(&lock);
                    return true;
                }
                else{
                    return false;
                }
            }
            return false;
        }
        bool Logout( int id_ )
        {
            std::unordered_map<int, Player>::const_iterator it = players.find(id_);
            if(it != players.end()){
                pthread_mutex_lock(&lock);
                OfflinePlayer(it->second);
                pthread_mutex_unlock(&lock);
                return true;
            }
            return false;
        }
        bool Match(int id_)
        {
            bool ret = false;
            std::unordered_map<int, Player>::iterator it = players.find(id_);
            if(it != players.end()){
                Player &player_ = it->second;
                pthread_mutex_lock(&lock);
                ret = MatchingPlayer(player_);
                pthread_mutex_unlock(&lock);
                if(ret){
                    ret = MatchBegin(player_);
                }
            }
            return ret;
        }

        int Game(int id_, int x_, int y_)
        {
            std::unordered_map<int, Player>::iterator it = players.find(id_);
            if(it != players.end()){
                Player &player_ = it->second;
                Room *room_ = player_.GetPlayRoom();
                return room_->Play(player_, x_, y_);
            }
        }
        ~GameManager()
        {
            pthread_mutex_destroy(&register_lock);
        }
};


#endif














