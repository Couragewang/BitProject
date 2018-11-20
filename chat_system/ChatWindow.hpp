#ifndef __CHAT_WINDOW_HPP__
#define __CHAT_WINDOW_HPP__

#include <iostream>
#include <string>
#include <string.h>
#include <ncurses.h>
#include <pthread.h>
#include "ChatClient.hpp"
//#include "ProtocolUtil.hpp"

class ChatWindow{
	private:
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
        static void *DrawWindow(void *arg)
        {
            int id = *(int*)arg;
            delete arg;
            switch(id){
            }
        }
        void Start(ChatClient *clip)
        {
            int i = 0;
            pthread_t tid;
            for( ; i < 4; i++ ){
                int *number = new int;
                *number = i;
                pthread_create(&tid, NULL, DrawWindow, (void *)number);
                threads.push_back(tid);
            }

            for(i=0; i < 4; i++){
                pthread_join(threads[i], NULL);
            }
        }
};

#endif



