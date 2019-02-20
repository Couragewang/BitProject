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
            return false;
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
        }
        bool IsCurrRight(const int &id)
        {
            return players[curr] == id;
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




