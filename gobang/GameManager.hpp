#ifndef _GAMEMANAGER_HPP_
#define _GAMEMANAGER_HPP_

#include <iostream>
#include <string>
#include <unordered_map>
#include <pthread.h>
#include "buttonrpc.hpp"
#include "PlayerManager.hpp"
#include "Protocol.hpp"

class GameManager{
    private:
        PlayerManager *pm;
        pthread_t match_thread;
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









