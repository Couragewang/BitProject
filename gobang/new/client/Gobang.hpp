#pragma once

#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include "NetClient.hpp"
#include "../codec.h"
#include "../server/Room.hpp"

using namespace rest_rpc;
using namespace rest_rpc::rpc_service;

class Gobang{
    private:
        std::string ip;
        std::string port;
    public:
        Gobang(std::string &ip_, std::string &port_):ip(ip_),port(port_)
        {}
        int Register(std::string nick_name_, std::string passwd_)
        {
	        try{
	        	boost::asio::io_service io_service;
	        	NetClient client(io_service);
	        	client.connect(ip, port);
	        	auto id = client.call<int>("Register", nick_name_, passwd_);
                return id;
	        }
	        catch (const std::exception& e){
	        	std::cout << e.what() << std::endl;
	        }
        }
        bool Login(int id_, std::string passwd_)
        {
	        try{
	        	boost::asio::io_service io_service;
	        	NetClient client(io_service);
	        	client.connect(ip, port);
	        	auto result = client.call<bool>("Login", id_, passwd_);
                return result;
	        }
	        catch (const std::exception& e){
	        	std::cout << e.what() << std::endl;
	        }
        }
        bool Logout(int id_, std::string passwd_)
        {
	        try{
	        	boost::asio::io_service io_service;
	        	NetClient client(io_service);
	        	client.connect(ip, port);
	        	auto result = client.call<bool>("Logout", id_, passwd_);
                return result;
	        }
	        catch (const std::exception& e){
	        	std::cout << e.what() << std::endl;
	        }
        }
        bool Match(int id_, std::string passwd_, int &room)
        {
	        try{
	        	boost::asio::io_service io_service;
	        	NetClient client(io_service);
	        	client.connect(ip, port);
	            room = client.call<int>("Match", id_, passwd_);
                if(room == -1){
                    return false;
                }
                return true;
	        }
	        catch (const std::exception& e){
	        	std::cout << e.what() << std::endl;
                return false;
	        }
        }
        std::string GetBoard(const int &room_id)
        {
	        try{
	        	boost::asio::io_service io_service;
	        	NetClient client(io_service);
	        	client.connect(ip, port);
	            return client.call<std::string>("GetBoard", room_id);
	        }
	        catch (const std::exception& e){
	        	std::cout << e.what() << std::endl;
            }
        }
        void ShowBoard(const std::string &board, const int &row, const int &col)
        {
            for(auto i = 0; i < row; i++){
                std::cout << " " << board[row*i + 0] << " | " << board[row*i + 1] << " | " <<\
                    board[row*i + 2] << " | " << board[row*i + 3] << " | " << board[row*i + 4] << std::endl;
                if( i != row - 1 ){
                    std::cout << "---|---|---|---|---" << std::endl;
                }
            }
        }
        void Move(int id, int room_id)
        {
            int x,y;
            std::cout << "Please Enter<x, y>: ";
            std::cin >> x >> y;
	        try{
	        	boost::asio::io_service io_service;
	        	NetClient client(io_service);
	        	client.connect(ip, port);
	            client.call("Move", id, room_id, x, y);
	        }
	        catch (const std::exception& e){
	        	std::cout << e.what() << std::endl;
            }
        }
        bool IsMyTurn(const int &id)
        {
	        try{
	        	boost::asio::io_service io_service;
	        	NetClient client(io_service);
	        	client.connect(ip, port);
	            return client.call<bool>("WhoPlay", id);
	        }
	        catch (const std::exception& e){
	        	std::cout << e.what() << std::endl;
            }
        }
        void Game(int id, int room_id)
        {
            std::string board = GetBoard(room_id);
            ShowBoard(board, BOARD_NUM, BOARD_NUM);
            for(;;){
                if(IsMyTurn(id)){
                    Move(id, room_id);
                    board = GetBoard(room_id);
                    ShowBoard(board, BOARD_NUM, BOARD_NUM);
                    //Judge(room);
                }
                else{
                    //Judge(room);
                    sleep(1);
                }
            }
        }
        ~Gobang()
        {
        }
};

