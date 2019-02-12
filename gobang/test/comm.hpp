#pragma once
#include <iostream>
#include <unistd.h>
#include "buttonrpc.hpp"

class test{
    public:
        int a[4][4];
        test()
        {
            for(auto i=0; i < 4; i++){
                for(auto j=0; j < 4; j++){
                    a[i][j] = i;
                }
            }
        }
        ~test()
        {
        }
        void show()
        {
            for(auto i=0; i < 4; i++){
                for(auto j=0; j < 4; j++){
                    std::cout << a[i][j] << ' ';
                }
                std::cout << std::endl;
            }
            sleep(100);
        }
};

test fun()
{
    test t;
    return t;
}









