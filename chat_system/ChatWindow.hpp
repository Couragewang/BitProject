#ifndef __CHAT_WINDOW_HPP__
#define __CHAT_WINDOW_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "ChatClient.hpp"
#include "ProtocolUtil.hpp"

class ChatWindow; 
class Param{
    public:
        ChatWindow *winp;
        ChatClient *clip;
        int number;
    public:
        Param(ChatWindow *winp_, int number_, ChatClient *clip_):winp(winp_), number(number_),clip(clip_)
        {}
};

class ChatWindow{
	public:
        WINDOW *header;
        WINDOW *output;
        WINDOW *list;
        WINDOW *input;
       // bool is_quit;
    private:
        std::vector<pthread_t> threads;
        pthread_mutex_t lock;
	public:
	    ChatWindow()
        {
	        header = NULL;
	        output = NULL;
	        list   = NULL;
	        input  = NULL;
            initscr();
            curs_set(0);
            pthread_mutex_init(&lock, NULL);
        }
        ~ChatWindow()
        {
	        delwin(header);
	        delwin(output);
	        delwin(list);
	        delwin(input);
            endwin();
            pthread_mutex_destroy(&lock);
        }
        WINDOW *GetHeader(){ return header;}
		WINDOW *GetOutput(){ return output;}
		WINDOW *GetList(){return list;}
		WINDOW *GetInput(){return input;}
		void DrawHeader()
        {
	        int h_ = LINES/5;
	        int w_ = COLS;
	        int y_  = 0;
	        int x_  = 0;
	        header = newwin(h_, w_, y_, x_);
	        box(header, 0, 0);//绘制边框
            pthread_mutex_lock(&lock);
	        wrefresh(header); //刷新窗口
            pthread_mutex_unlock(&lock);
        }
        void HeaderYX(int &y_, int &x_)
        {
            getmaxyx(header, y_, x_);
        }
        void OutputYX(int &y_, int &x_)
        {
            getmaxyx(output, y_, x_);
        }
        void ListYX(int &y_, int &x_)
        {
            getmaxyx(list, y_, x_);
        }
		void DrawList()
        {
	        int h_ = LINES*3/5;
	        int w_ = COLS/4;
	        int y_ = LINES/5;
	        int x_ = COLS*3/4;
	        list = newwin(h_, w_, y_, x_);
	        box(list, 0, 0);//绘制边框
            pthread_mutex_lock(&lock);
	        wrefresh(list); //刷新窗口
            pthread_mutex_unlock(&lock);
        }
		void DrawOutput()
        {
	        int h_ = (LINES*3)/5;
	        int w_ = (COLS*3)/4;
	        int y_ = LINES/5;
	        int x_ = 0;
	        output = newwin(h_, w_, y_, x_);
	        box(output, 0, 0);//绘制边框
            pthread_mutex_lock(&lock);
	        wrefresh(output); //刷新窗口
            pthread_mutex_unlock(&lock);
        }
		void DrawInput()
        {
	        int h_ = LINES/5;
	        int w_ = COLS;
	        int y_ = 4*LINES/5;
	        int x_ = 0;
	        input = newwin(h_, w_, y_, x_);
	        box(input, 0, 0);//绘制边框
            pthread_mutex_lock(&lock);
	        wrefresh(input); //刷新窗口
            pthread_mutex_unlock(&lock);
        }
		void PutStringToWin(WINDOW *win_, int y_, int x_, const std::string &string_)
        {
	        mvwaddstr(win_, y_, x_, string_.c_str());
            pthread_mutex_lock(&lock);
	        wrefresh(win_); //刷新窗口
            pthread_mutex_unlock(&lock);
        }
		void GetStringToWin(WINDOW *win_, std::string &string_)
        {
        //	char buffer_[MESSAGE_SIZE];
        	char buffer_[1024];
        	memset(buffer_, '\0', sizeof(buffer_));
        	wgetnstr(win_, buffer_, sizeof(buffer_));
        	string_ = buffer_;
        }
		void ClearWinLine(WINDOW *win_, int begin_, int num_)
        {
	        while( num_-- > 0 ){
	        	wmove(win_, begin_++, 0);
	        	wclrtoeol(win_);//clrtoeol是从光标位置清除到光标所在行的结尾
	        }
        }
        static void RunHeader(ChatWindow *winp_)
        {
            int y_,x_;
            std::string welcome_ = "Welcome To Our Chat System";
            int pos_ = 1;
            int dir = 0;
            for( ; ; ){
                winp_->DrawHeader();
                winp_->HeaderYX(y_, x_);
                winp_->PutStringToWin(winp_->GetHeader(), y_/2, pos_, welcome_);
                if(pos_ > x_ - welcome_.size() - 2){
                    dir = 1;
                }
                else if(pos_ < 2){
                    dir = 0;
                }

                if(dir == 0){
                    pos_++;
                }
                else{
                    pos_--;
                }
                sleep(1);
            }
        }
        static void RunOutput(ChatWindow *winp_, ChatClient *clip_)
        {
            std::string message_;
            Message msg;
            int line = 1;
            int y_,x_;
            winp_->DrawOutput();
            for( ; ; ){
                winp_->OutputYX(y_, x_);
                clip_->RecvMessage(message_);
                msg.Deserialize(message_);

                std::string show_message;
                show_message = msg.GetNickName();
                show_message += "-";
                show_message += msg.GetSchool();
                show_message += "# ";
                show_message += msg.GetMsg();
                if(line > y_ - 2){
                    line = 1;
                    winp_->DrawOutput();
                }
                //std::cout << show_message << std::endl;
                winp_->PutStringToWin(winp_->GetOutput(), line++, 1, show_message);

                //也可以写入本地文件，进行持久化
                std::string user;
                user = msg.GetNickName();
                user += "-";
                user += msg.GetSchool();

                clip_->PushUser(user);
            }
        }
        static void RunList(ChatWindow *winp_, ChatClient *clip_)
        {
            int y_, x_;
            for( ; ; ){
                winp_->DrawList();
                winp_->ListYX(y_, x_);
                std::vector<std::string> &user = clip_->GetUser();
                std::vector<std::string>::iterator it = user.begin();
                for( ; it != user.end(); it++){
                    int line = 1;
                    winp_->PutStringToWin(winp_->GetList(), line++, 1, *it);
                }
                sleep(2);
            }

        }
        static void RunInput(ChatWindow *winp_, ChatClient *clip_)
        {
            std::string enter_msg_;
            Message msg;
            std::string send_message_;
            std::string tips = "Please Enter# ";
            Me &myself = clip_->GetMySelf();
            msg.SetNickName(myself.nick_name);
            msg.SetSchool(myself.school);
            msg.SetId(myself.id);
            for( ; ; ){
                winp_->DrawInput();
                winp_->PutStringToWin(winp_->GetInput(), 2, 2, tips);
                winp_->GetStringToWin(winp_->GetInput(), enter_msg_);
                msg.SetMsg(enter_msg_);
                msg.Serialize(send_message_);
                clip_->SendMessage(send_message_);
            }

        }
        static void *DrawWindow(void *arg)
        {
            Param *param_ = (Param*)arg;

            ChatWindow *winp_ = param_->winp;
            ChatClient *clip_ = param_->clip;
            int number_ = param_->number;
            switch(number_){
                case 0:
                    RunHeader(winp_);
                    break;
                case 1:
                    RunOutput(winp_, clip_);
                    break;
                case 2:
                    RunList(winp_, clip_);
                    break;
                case 3:
                    RunInput(winp_, clip_);
                    break;
                default:
                    break;
            }
            delete param_;
        }
        void Start(ChatClient *clip)
        {
            int i = 0;
            pthread_t tid;
            int num = 4;
            for( ; i < num; i++ ){
                Param *p = new Param(this, i, clip);
                pthread_create(&tid, NULL, DrawWindow, (void *)p);
                threads.push_back(tid);
            }

            for(i=0; i < num; i++){
                pthread_join(threads[i], NULL);
            }
        }
};

#endif



