#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>

#define WAIT_TIME 30
class Client{
    private:
        int sock;
        std::string ip;
        int port;
    public:
        Client(std::string &ip_, int &port_)
            :sock(-1),ip(ip_),port(port_)
        {
        }
        //与大厅交互
        void InteractHall(char cmd, std::string &msg)
        {
            sock = SocketApi::Socket();
            SocketApi::Connect(sock, ip, port);
            write(sock, &cmd, 1);
            SocketApi::Send(sock, msg);
            SocketApi::Recv(sock, msg);
            close(sock);
        }
        bool Register(std::string nick_name_, std::string passwd_, int &id)
        {
            std::string msg;
            Register_Request reg = {nick_name_.c_str(), passwd_.c_str()};
            rr.Serialize(msg);

            InteractHall('r', msg);

            Response r = {-1, -1};
            r.Deserialize(msg);
            if(r.status == 0){
                std::cout << "Register Done, Your Login Id Is : " << r.player_id << std::endl;
                id = r.player_id;
                return true;
            }else{
                std::cout << "Register Failed!" << std::endl;
                return false;
            }
        }
        bool Login(int id, std::string passwd)
        {
            std::string msg;
            LL_Request lr = {id, passwd};
            lr.Serialize(msg);

            InteractHall('l', msg);

            Response r = {-1, -1};
            r.Deserialize(msg);
            if(r.status == 0){
                std::cout << "Login Success!" << r.player_id << std::endl;
                return true;
            }else{
                std::cout << "Login Failed" << std::endl;
                return false;
            }
        }
        bool Logout(int id, std::string passwd)
        {
            std::string msg;
            LL_Request lr = {id, passwd};
            lr.Serialize(msg);

            InteractHall('o', msg);

            Response r = {-1, -1};
            r.Deserialize(msg);
            if(r.status == 0){
                std::cout << "Logout Success!" << r.player_id << std::endl;
                return true;
            }else{
                std::cout << "Logout Failed" << std::endl;
                return false;
            }
        }
        void Match()
        {

        }
        void Game()
        {

        }
        ~Client()
        {
        }
};

//void Play(buttonrpc &client, int &id, char &chess_color)
//{
//    int x,y;
//    volatile bool is_quit = false;
//    while(!is_quit){
//        std::cout << "Please Enter Your Pos<x,y>: ";
//        std::cin >> x >> y;
//
//        switch(client.call<int>("Game",id, x, y).val()){
//            case -1:
//                std::cout << "It is not your turn yet!" << std::endl;
//                break;
//            case -2:
//                std::cout << "Pos be occupied!" << std::endl;
//                break;
//            case -3:
//                std::cout << "Pos is not right!" << std::endl;
//                break;
//            case 0:
//                {
//                    room_ = client.call<Room>("GetRoom", id).val();
//                    char result = room_.Judge();
//                    if(result != 'N'){
//                        if(result == 'E'){
//                            std::cout << "Tie Over!" << std::endl;
//                        }
//                        if(result == chess_color){
//                            std::cout << "You Win!" << std::endl;
//                        }
//                        else{
//                            std::cout << "You Lose!" << std::endl;
//                        }
//                        client.call<void>("GameEnd", id);
//                        return;
//                    }
//                }
//                break;
//            default:
//                break;
//        }
//    }
//}
//void Game(buttonrpc &client, int &id)
//{
//    int select;
//    int x,y;
//    volatile bool end = false;
//    while(!end){
//        Menu2();
//        std::cin >> select;
//        switch(select){
//            case 1:
//                {
//                    std::cout << "Match Begin!" << std::endl;
//                    pthread_t tid;
//                    pthread_create(&tid, NULL, Count, NULL);
//                    if(client.call<bool>("Match", id).val()){
//                        pthread_cancel(tid);
//                        pthread_join(tid, NULL);
//                        std::cout << "Match Success!" << std::endl;
//                        char chess_color = client.call<char>("PlayerChessColor", id).val();
//                        std::cout << "Your Chess Color is : " << chess_color << std::endl;
//                        Play(client, id, chess_color);
//                    }else{
//                        pthread_join(tid, NULL);
//                        std::cout << "Match Failed!" << std::endl;
//                    }
//                }
//                break;
//            case 2:
//                client.call<bool>("Logout", id).val();
//                std::cout << "Logout Success" << std::endl;
//                return;
//            default:
//                break;
//        }
//    }
//}
#endif
