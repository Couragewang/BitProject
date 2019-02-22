#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include "../codec.h"

#define BOARD_NUM 5

class Room{
    public:
        //基本信息
        char board[BOARD_NUM][BOARD_NUM];
        int players[2];
        char chess_color[2];
        int curr;
        //结果信息
        int who_win;

	    MSGPACK_DEFINE(board, players,chess_color, curr, who_win);
    public:
        Room()
        {}
        Room(int p0_, int p1_)
            : curr(0)
        {
            players[0] = p0_;
            chess_color[0] = 'X';
            players[1] = p1_;
            chess_color[1] = 'O';
            who_win = -1;
            memset(board, ' ', sizeof(board));
        }
        bool IsSafe(const int &id, int x, int y)
        {
            bool ret = false;
            if(players[curr] == id \
                    && x >=0 \
                    && y >= 0\
                    && x <= BOARD_NUM-1 \
                    && y <= BOARD_NUM-1 \
                    && board[x][y] == ' '){
                ret = true;
            }
            std::cout << "curr: " << curr << std::endl;
            std::cout << "id: " << id << std::endl;
            std::cout << "x: " << x << std::endl;
            std::cout << "y: " << y << std::endl;
            std::cout << "board[x][y]: " << board[x][y] << std::endl;
            return ret;
        }
        void PlayChess(const int &x, const int &y)
        {
            board[x][y] = chess_color[curr];
        }
        void ChagePlayer()
        {
            if(curr == 0){
                curr = 1;
            }else{
                curr = 0;
            }
            std::cout << "curr play is : " << curr << std::endl;
        }
        bool IsCurrRight(const int &id)
        {
            return players[curr] == id;
        }
        bool IsFull()
        {
            for(auto i = 0; i < BOARD_NUM; i++){
                for(auto j = 0; j < BOARD_NUM; j++){
                    if(board[i][j] == ' '){
                        return false;
                    }
                }
            }
            return true;
        }
        int WhoWin()
        {
            char c = 'N';
            for(auto i = 0; i < BOARD_NUM; i++){
                if(board[i][0] == board[i][1] && \
                   board[i][1] == board[i][2] && \
                   board[i][2] == board[i][3] && \
                   board[i][3] == board[i][4] && \
                   board[i][3] != ' '){
                    c = board[i][0];
                    break;
                }
                if(board[0][i] == board[1][i] && \
                   board[1][i] == board[2][i] && \
                   board[2][i] == board[3][i] && \
                   board[3][i] == board[4][i] && \
                   board[4][i] != ' '){
                    c = board[0][i];
                    break;
                }
            }
            if((board[0][0] == board[1][1] && board[1][1] == board[2][2] &&\
                board[2][2] == board[3][3] && board[3][3] == board[4][4] && board[2][2] != ' ') || \
                (board[0][4] == board[1][3] && board[1][3] == board[2][2] && \
                     board[2][2] == board[3][1] && board[3][1] == board[4][0] && board[2][2] != ' ') ){
                c = board[2][2];
            }
            if(c != 'N' && IsFull()){
                c = 'F';
            }
            int who_win = 0;
            switch(c){
                case 'N':
                    who_win = 0;
                    break; //Next
                case 'F':
                    who_win = 1; //Full 平局
                    break;
                case 'X':
                    who_win = players[0];
                    break;
                case 'O':
                    who_win = players[1];
                    break;
                default:
                    return -1;
            }
            return who_win;
        }
        std::string GetBoard()
        {
            std::string board_string;
            for(auto i = 0; i < BOARD_NUM; i++){
                for(auto j = 0; j < BOARD_NUM; j++){
                    board_string.push_back(board[i][j]);
                }
            }
            return board_string;
        }
        ~Room()
        {
        }
};




