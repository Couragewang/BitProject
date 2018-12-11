#ifndef _GAMEMANAGER_HPP_
#define _GAMEMANAGER_HPP_

#include <iostream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <pthread.h>
#include "buttonrpc.hpp"
#include "PlayerManager.hpp"
#include "Comm.hpp"

#define ROW 5
#define COL 5

class GameManager{
    private:
        PlayerManager *pm;
        pid_t match_thread;
    public:
        GameManager(PlayerManager *pm_):pm(pm_)
        {}
        bool ThreadMatchRun()
        {
            pm->MatchService();
        }
        static void *MatchService(void *arg)
        {
            pthread_detach(pthread_self());
            GameManager *gm = (GameManager*)arg;
            for( ; ; ){
                //gm->WaitMatch();
                gm->ThreadMatchRun();
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









