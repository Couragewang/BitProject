#ifndef __CHAT_WINDOW_HPP__
#define __CHAT_WINDOW_HPP__

#include <iostream>
#include <string>
#include <string.h>
#include <ncurses.h>
#include <pthread.h>
#include "ChatClient.hpp"
#include "ProtocolUtil.hpp"

class ChatWindow;

class param{
    public:
        ChatWindow *winp;
        ChatClient *clip;
        int number;
    public:
        ChatWindow(ChatWindow *winp_, int number_, ChatClient *clip_):winp(winp_), number(number_),clip(clip_)
        {}
};

class ChatWindow{
	public:
        WINDOW *header;
        WINDOW *output;
        WINDOW *list;
        WINDOW *input;
    private:
        std::vector<pthread_t> threads;
	public:
	    ChatWindow()
        {
	        header = NULL;
	        output = NULL;
	        list   = NULL;
	        input  = NULL;
            initscr();
            curs_set(0);
        }
        ~ChatWindow()
        {
	        delwin(header);
	        delwin(output);
	        delwin(list);
	        delwin(input);
            endwin();
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
	        wrefresh(header); //刷新窗口
        }
        void HeaderYX(int &y_, int &x_)
        {
            getmaxyx(header, y_, x_);
        }
        void OutputYX(int &y_, int &x_)
        {
            getmaxyx(output, y_, x_);
        }
		void DrawList()
        {
	        int h_ = LINES*3/5;
	        int w_ = COLS/4;
	        int y_ = LINES/5;
	        int x_ = COLS*3/4;
	        list = newwin(h_, w_, y_, x_);
	        box(list, 0, 0);//绘制边框
	        wrefresh(list); //刷新窗口
        }
		void DrawOutput()
        {
	        int h_ = (LINES*3)/5;
	        int w_ = (COLS*3)/4;
	        int y_ = LINES/5;
	        int x_ = 0;
	        output = newwin(h_, w_, y_, x_);
	        box(output, 0, 0);//绘制边框
	        wrefresh(output); //刷新窗口
        }
		void DrawInput()
        {
	        int h_ = LINES/5;
	        int w_ = COLS;
	        int y_ = 4*LINES/5;
	        int x_ = 0;
	        input = newwin(h_, w_, y_, x_);
	        box(input, 0, 0);//绘制边框
	        wrefresh(input); //刷新窗口
        }
		void PutStringToWin(WINDOW *win_, int y_, int x_, const std::string &string_)
        {
	        mvwaddstr(win_, y_, x_, string_.c_str());
	        wrefresh(win_); //刷新窗口
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
                winp->HeaderYX(y_, x_);
                winp_->PutStringToWin(winp_->GetHeader(), y_/2, pos_, welcome_);
                if(pos_ > x_ - welcome_.size() - 1){
                    dir = 1;
                }
                else if(pos_ < 1){
                    dir = 0;
                }

                if(dir == 0){
                    pos_++;
                }
                else{
                    pos_--;
                }
                usleep(200000);
            }
        }
        static void RunOutput(ChatWindow *winp_, ChatClient *clip_)
        {
            std::string message_;
            Message msg;
            int line = 1;
            int y_,x_;
            for( ; ; ){
                winp_->DrawOutput();
                winp->OutputYX(y_, x_);
                clip_->RecvMessage(message_);
                msg.Deserialize(message_);
                std::string show_message;
                show_message = msg.GetNickName();
                show_message += "-";
                show_message += msg.GetSchool();
                show_message += "# ";
                show_message += msg.GetMsg();
                
                if(line > y_ - 1){
                    line = 1;
                    winp_->DrawOutput();
                }
                winp_->PutStringToWin(winp_->GetOutput(), line++, 1, show_message);

                std::string user;
                user = msg.GetNickName();
                user += "-";
                user += msg.GetSchool();


            }
        }
        static void RunList(ChatWindow *winp_)
        {
            for( ; ; ){
                winp->DrawList();
            }

        }
        static void RunInput(ChatWindow *winp_, ChatClient *clip_)
        {
            std::string message_;
            Message msg;
            for( ; ; ){
                winp->DrawInput();
                clip_->SendMessage(message_);
            }

        }
        static void *DrawWindow(void *arg)
        {
            param *param_ = (param*)arg;

            ChatWindow *winp_ = param_->winp;
            ChatClient *clip_ = param_->clip;
            int number_ = param_->number;

            delete param_;
            switch(number){
                case 0:
                    RunHeader(winp_);
                    break;
                case 1:
                    RunOutput(winp_, clip_);
                    break;
                case 2:
                    Runlist(winp_);
                    break;
                case 3:
                    RunInput(winp_, clip_);
                    break;
                default:
                    break;
            }
        }
        void Start(ChatClient *clip)
        {
            int i = 0;
            pthread_t tid;
            int num = 4;
            for( ; i < num; i++ ){
                param *p = new param(this, i, clip);
                pthread_create(&tid, NULL, DrawWindow, (void *)p);
                threads.push_back(tid);
            }

            for(i=0; i < num; i++){
                pthread_join(threads[i], NULL);
            }
        }
};

#endif



