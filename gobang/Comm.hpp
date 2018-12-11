#ifndef _COMM_H_
#define _COMM_H_

#include <iostream>

class Room{
    private:
        char board[ROW][COL];
        int curr_id;
        int player_one;
        int player_two;
        int who_win;
        enum{
            FULL,
            DESTROY,
        }status;

        bool IsPosLegal(const int &x_, const int &y_)
        {
            return board[x_-1][y_-1] == ' ';
        }
        bool IsPlayerRight(int id_)
        {
            return curr_id == id_;
        }
        void SwitchPlayer(int id_)
        {
            if(curr_id == player_one){
                curr_id = player_two;
            }
            else{
                curr_id = player_one;
            }
        }
    public:
        Room(int id1_, int id2_):
            player_one(id1_), player_two(id2_), curr_id(id1_), who_win(-1),status(FULL)
        {
            memset(board, ' ', sizeof(char)*ROW*COL);
        }
        int Game(int id_, int x_, int y_, char color_)
        {
            if(!IsPlayerRight(id_)){
                return -1;
            }
            int result = 0;
            if( x_ >= 1 && x <= 5 && y_ >= 1 && y_ <= 5){
                if(IsPosLegal(x_, y_)){
                    board[x_ - 1][y_ - 1] = color_;
                }
                else{
                    result = -2;
                }
            }
            else{
                result = -3;
            }
            return result;
        }
        int WhoWin()
        {
            for(auto i_ = 0; i_ < ROW; i_++){
                if( (board[i_][0] != ' ')&&\
                    (board[i_][0] == board[i_][1]) &&\
                    (board[i_][1] == board[i_][2]) &&\
                    (board[i_][2] == board[i_][3]) &&\
                    (board[i_][3] == board[i_][4]) ){
                    return board[i_][0];
                }
                if( (board[0][i_] != ' ') &&\
                    (board[0][i_] == board[1][i_]) &&\
                    (board[1][i_] == board[2][i_]) &&\
                    (board[2][i_] == board[3][i_]) &&\
                    (board[3][i_] == board[4][i_]) ){
                    return board[0][i_];
                }
            }
            if( (board[0][0] != ' ') &&\
                (board[0][0] == board[1][1]) &&\
                (board[1][1] == board[2][2]) &&\
                (board[2][2] == board[3][3]) &&\
                (board[3][3] == board[4][4]) ){
                    return board[0][0];
            }
            if( (board[0][4] != ' ') &&\
                (board[0][4] == board[1][3]) &&\
                (board[1][3] == board[2][2]) &&\
                (board[2][2] == board[3][1]) &&\
                (board[3][1] == board[4][0]) ){
                return board[0][4];
            }
            for(auto i_ = 0; i_ < ROW; i_++){
                for(auto j_ = 0; j_ < COL; j_++){
                    if(board[i_][j_] == ' '){
                        return 'N';
                    }
                }
            }
            return 'E';
        }
        void GameEnd(int id_)
        {
            if(player_one == id_){
                player_one = -1;
            }
            else
            {
                player_two = -1;
            }
            if(player_one == -1 && player_two == -1){
                status = DESTROY;
            }
        }
        ~Room()
        {
        }
};

#endif
