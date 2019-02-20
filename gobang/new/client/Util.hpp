#pragma once

#include <iostream>
#include <string>
#include <unistd.h>

#define WAIT_TIME 30

class Util{
    public:
        static void *Count(void *arg)
        {
            int time = 0;
            while(time < WAIT_TIME){
                std::cout << "Matching... [ " << time << "/" << WAIT_TIME << " ]S\r";
                fflush(stdout);
                sleep(1);
                time++;
            }
            std::cout << std::endl;
        }
        static void Usage(std::string proc)
        {
            std::cout << proc << " ip port" << std::endl;
        }
        static void Menu1()
        {
            std::cout << "###########################################" << std::endl;
            std::cout << "##  1. register               2. login   ##" << std::endl;
            std::cout << "##                            3. exit    ##" << std::endl;
            std::cout << "###########################################" << std::endl;
            std::cout << "Please Select# ";
        }
        static void Menu2()
        {
            std::cout << "###########################################" << std::endl;
            std::cout << "## 3. Match                   4. logout  ##" << std::endl;
            std::cout << "###########################################" << std::endl;
            std::cout << "Please Select# ";
        }
        static void RegisterEnter(std::string &name, std::string &passwd)
        {
            std::string p1,p2;
            std::cout << "Please Enter Nick Name: ";
            std::cin >> name;
            std::cout << "Please Enter Passwd: ";
            std::cin >> p1;
            std::cout << "Please Enter Passwd Again: ";
            std::cin >> p2;
            if(p1 != p2){
                std::cout << "Enter Error" << std::endl;
                exit(2);
            }
            passwd = p1;
        }
        static void LoginEnter(int &id, std::string &passwd)
        {
            std::cout << "Please Enter Login Id: ";
            std::cin >> id;
            std::cout << "Please Enter Passwd: ";
            std::cin >> passwd;
        }
};
