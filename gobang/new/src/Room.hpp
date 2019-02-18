#ifndef _ROOM_HPP_
#define _ROOM_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include "Protocol.hpp"

#define BOARD_NUM 25

class Room{
    public:
        //基本信息
        char board[BOARD_NUM];
        int plays[2];
        int master; 
        //结果信息
        int who_win;
    public:
        Room(int p0_, int p1_)
            : master(p0_)
        {
            plays[0] = p0_;
            plays[1] = p1_;
            who_win = -1;
            memset(board, ' ', sizeof(board));
        }
        ~Room()
        {
        }
};

#endif
