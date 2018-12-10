#ifndef _GAMEMANAGER_HPP_
#define _GAMEMANAGER_HPP_

#include <iostream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <pthread.h>
#include "buttonrpc.hpp"
#include "PlayerManager.hpp"

#define ROW 5
#define COL 5

class GameManager{
    private:
        PlayerManager pm;
        pid_t match_thread;
    public:
        GameManager()
        {}
        int Register(std::string nick_name_, std::string passwd_)
        {
            return pm.InsertNewPlayer(nick_name_, passwd_);
        }
        bool Login( int id_, std::string passwd_ )
        {
            bool ret = false;
            if(IsPlayerLegal(id_, passwd_)){
                ret = pm.Online(id_);
            }
            return ret;
        }
        bool Logout( int id_ )
        {
            return pm.Offline();
        }
        bool Match(int id_)
        {
            bool ret = false;
            if(pm.PushMatchPool(id_)){
                ret = pm.PlayerWait(id_);
                if(ret){
                    pm.PopMatchingPool(id_);
                }
            }
            return ret;
        }
        int Game(int id_, int x_, int y_)
        {
            int result = pm.Play(id_, x_, y_);
            return result;
        }
        int WhoWin(int id_)
        {
            return pm.WhoWin(id_);
        }
        //match thread run
        bool MatchRun()
        {
            pm.Match();
        }
        static void *MatchService(void *arg)
        {
            pthread_detach(pthread_self());
            GameManager *gm = (GameManager*)arg;
            for( ; ; ){
                //gm->WaitMatch();
                gm->MatchRun();
            }
        }
        void StartMatchThread()
        {
            pthread_create(&match_thread, NULL, MatchService, (void*)this);
        }
        ~GameManager()
        {}
};
#endif









