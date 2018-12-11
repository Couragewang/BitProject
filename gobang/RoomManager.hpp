#ifndef _ROOM_MANAGER_HPP_
#define _ROOM_MANAGER_HPP_

#include <iostream>
#include <string>
#include <unordered_map>
#include "Comm.hpp"

#define ROOM_NUM 100

class RoomMamager{
    private:
        std::unordered_map<int, Room> game_hall;
        int assign_id;
        pthread_mutex_t lock;
    public:
        void LockRoom()
        {
            pthread_mutex_lock(&lock);
        }
        void UnlockRoom()
        {
            pthread_mutex_unlock(&lock);
        }
    public:
        RoomManager():assign_id(0)
        {
            pthread_mutex_init(&lock, NULL);
        }
        int CreateRoom(int id1_, int id2_)
        {
            Room room_(id1_, id2_);
            LockRoom();
            int room_id_ = assign_id++;
            game_hall.insert({room_id_, room_});
            UnlockRoom();
            return room_id_;
        }
        int Game(int room_id_, int id_, int x_, int y_, char color_)
        {
            return game_hall[room_id_].Game(id_, x_, y_, color_);
        }
        int DestroyRoom(int id_)
        {
            LockRoom();
            game_hall.erase(id_);
            UnlockRoom();
        }
        Room GetRoom(int room_id_)
        {
            LockRoom();
            Room room_ = game_hall[room_id_];
            UnlockRoom();
            return room_;
        }
        void GameEnd(int room_id_, int id_)
        {
            LockRoom();
            game_hall[room_id_].GameEnd(id_);
            UnlockRoom();
        }
        ~RoomManager()
        {
            pthread_mutex_destroy(&lock);
        }
};

#endif
