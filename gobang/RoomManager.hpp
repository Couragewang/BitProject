#ifndef _ROOM_MANAGER_HPP_
#define _ROOM_MANAGER_HPP_

#include <iostream>
#include <string>
#include <unordered_map>

#define ROOM_NUM 100

class Room{
    private:
        char board[ROW][COL];
        int curr_id;
        int player_one;
        int player_two;
        int who_win;

        bool IsPosLegal(const int &x_, const int &y_)
        {
            return board[x_-1][y_-1] == ' ';
        }
        bool IsPlayerRight(int id_)
        {
            return curr_id == id_;
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
        Room(int id1_, int id2_):
            player_one(id1_), player_two(id2_), curr_id(id1_), who_win(-1)
        {
            memset(board, ' ', sizeof(char)*ROW*COL);
        }
        int Play(int id_, int x_, int y_)
        {
            if(!IsPlayerRight(id_)){
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
        int CreateRoom(int id1_, int id2_)
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

#endif
