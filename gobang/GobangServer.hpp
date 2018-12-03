#ifndef _GOBANG_HPP_
#define _GOBANG_HPP_

#include <iostream>
#include <string>
#include "buttonrpc.hpp"

#define ROW 5
#define COL 5

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

class Player{
    private:
        int id;
        status_t status;
        std::string nick_name;
        chessman_t chessman;
        int wins;
        int loses;
        int ties;
        //逃跑
    public:
        Player(const int &id_, const std::string &nick_name_):
            id(id_), status(OFFLINE), nick_name(nick_name_),wins(0),loses(0),ties(0),checcman(NONE)
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
        int GetId()
        {
            return id;
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
        bool Judge(Player &player_)
        {
            for(auto i_ = 0; i_ < ROW; i_++){
                for(auto j_ = 0; j_ < COL, j_++){

                }
            }
        }
    public:
        Room(Player &one_, Player &two_):player_one(one_), player_two(two_), curr_id(player_one.GetId()), who_win(-1)
        {
            for(auto i=0; i < ROW; i++){
                for(auto j=0; j < COL; j++){
                    board[i][j] = ' ';
                }
            }

        }
        int Play(Player &player_, int x_, int y_)
        {
            if(!IsPlayerRight(player_)){
                return -1;
            }
            if( x_ >= 1 && x <= 5 && y_ >= 1 && y_ <= 5){
                if(IsPosLegal(x_, y_)){
                    char color_ = player_.GetChessColor();
                    board[x_ - 1][y_ - 1] = color_;
                    switch(Judge(player_)){
                        case 'e': //eq
                            return 0;
                        case 'w': //win
                            return player_.GetId();
                        case 'n': //normal
                            SwitchPlayer(player_);
                            break;
                        default:
                            break;
                    }
                }
                else{
                    return -2;
                }
            }
        }
        ~Room()
        {
        }
};



#endif














