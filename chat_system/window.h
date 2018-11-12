#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>
#include <string>
#include <ncurses.h>
#include <string.h>

class chat_window{
	public:
		chat_window();
		~chat_window();
		void init();
		void draw_header();
		void draw_output();
		void draw_list();
		void draw_input();
		static void win_refresh(WINDOW* _win);
		static void put_str_to_win(WINDOW *_win, int _y, int _x, const std::string &_str);
		static void get_str_to_win(WINDOW *_win, std::string &_msg);
		static void clear_win_line(WINDOW *_win, int begin, int num);


		WINDOW *get_header(){ return this->header;}
		WINDOW *get_output(){ return this->output;}
		WINDOW *get_list(){return this->list;}
		WINDOW *get_input(){return this->input;}

	private:
		WINDOW *create_win(int hei, int wth, int y, int x);

	private:
		WINDOW *header;
		WINDOW *output;
		WINDOW *list;
		WINDOW *input;
};


#endif
