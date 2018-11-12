#include <pthread.h>
#include "client_main.h"
#include "udp_client.h"
#include "udp_data.h"
#include "window.h"

static chat_window win;
static udp_client cli;

void *header_run(void* arg)
{
	std::string _msg = "Welcome bit!";
	win.draw_header();
	WINDOW* h = win.get_header();
	int x,y;
	x=0;
	while(1){
		int max_y,max_x;
		getmaxyx(h, max_y, max_x);
		y = max_y/2;
		win.clear_win_line(h, y, 1);
		win.put_str_to_win(h, y, ++x%max_x, _msg);
		usleep(100000);
		win.win_refresh(h);
	}
}

void *output_run(void* arg)
{
	std::string _msg;
	win.draw_output();
	WINDOW *o = win.get_output();
	int x = 5;
	int y = 0;
	udp_data data;
	while(1){
		int max_y,max_x;
		getmaxyx(o, max_y, max_x);
		y++;
		y = y%max_y;
		if( y == 0 ){
			win.clear_win_line(o, 1, max_y-1);
			continue;
		}

		win.win_refresh(o);
		ssize_t ret = cli.recv_msg(_msg); //get msg
		if( ret > 0 ){
			data.str_to_val(_msg);
			std::string _nick_name = data.get_nick_name();
			std::string _school     = data.get_school();
			_msg                    = data.get_msg();
			std::string _type      = data.get_type();
			std::string _cmd       = data.get_cmd();

			std::string _output    = "[";
			_output                += _nick_name;
			_output                += "/";
			_output                += _school;
			_output                += "]$ ";
			_output                += _msg;
			win.put_str_to_win(o, y, x, _output);
			std::string friend_key=_nick_name + "/" + _school;
			if( _type == "CMD" && _cmd == "QUIT"){
				cli.del_friend(friend_key);
			}else{
				cli.add_friend(friend_key);
			}
		}
		win.win_refresh(o);
	}
}

void *list_run(void* arg)
{
	std::string _msg = "hello bit";
	win.draw_list();
	WINDOW *l = win.get_list();
	int x = 2;
	while(1){
		int max_y,max_x;
		getmaxyx(l, max_y, max_x);

		std::vector<std::string> _vec=cli.get_friends_list();
		int friends_num = _vec.size();
		int page = max_y-3;

		int page_num = friends_num/page;
		int page_mod = friends_num%page;
		if( page_mod > 0 ){
			page_num++;
		}

		std::strstream ss;
		std::string str_page_num;
		ss<<page_num;
		ss>>str_page_num;
		std::string _out_page;
		win.win_refresh(l);
		for(int i=0; i<page_num; i++){
			std::string str_i;
			std::strstream ssi;
			ssi<<i+1;
			ssi>>str_i;		
			_out_page= str_i+"/"+str_page_num;
		    win.clear_win_line(l, max_y-2, 1);
		    win.put_str_to_win(l, max_y-2, max_x/2, _out_page);

			int y = 0;
			for(int j=0; j<page; j++){
				y++;
				y=y%(max_y-3);
				int index = i*page+j;
				if( index < friends_num){
					std::string _fri = _vec[index];
		            if(y == 0){
		                win.win_refresh(l);
						sleep(3);
		            	win.clear_win_line(l, 1, max_y-4);
		            	continue;
		            }
		            win.put_str_to_win(l, y, x, _fri);
				}else{
					break;
				}
			}
		}
		sleep(5);
	}
}

static udp_data in_data;

void *input_run(void* arg)
{
	const std::string msg = "Enter$ ";
	win.draw_input();
	WINDOW *i = win.get_input();
	int _y = 1;
	int _x = 5;
	std::string input;
	while(1){
		input="";
		win.put_str_to_win( i, _y, _x, msg );
		win.win_refresh(i);
		win.get_str_to_win(i, input);
		if(input == "quit"){
		    in_data.set_msg("None");
			in_data.set_type("CMD");
			in_data.set_cmd("QUIT");
		    std::string json_msg;
		    in_data.val_to_str(json_msg);
			cli.send_msg(json_msg);
			exit(0);
		}
		//send
		/////////for test///////////
		in_data.set_msg(input);
		std::string json_msg;
		in_data.val_to_str(json_msg);
		////////////////////////////

		cli.send_msg(json_msg);
		//delete
		int max_y,max_x;
		getmaxyx(i, max_y, max_x);
		win.clear_win_line(i, 1, max_y-1);
		usleep(100000);
	}
}

void usage()
{
	std::cout<<"udp_client -i 127.0.0.1 -p 8888"<<std::endl;
}

void get_opt(int argc, char *argv[])
{
	//system("clear");
	char *opt_str="hi:p:";
	int ch;
	int flag = 0;
	while((ch = getopt(argc, argv, opt_str)) != -1 ){
		switch(ch){
			case 'h':
				usage();
				exit(1);
				break;
			case 'i':
				{
					flag++;
			    	std::string _ip = optarg;
					cli.set_ip(_ip);
				}
				break;
			case 'p':
				{
					flag++;
					int _p = atoi(optarg);
					cli.set_port((unsigned short int)_p);
				}
				break;
			default:
				usage();
				exit(1);
				break;
		}
	}
	if(flag != 2){
		usage();
		exit(1);
	}
}

int main(int argc ,char *argv[])
{
	get_opt(argc, argv);
	cli.init();
	win.init();

	std::string _nick_name;
	std::string _school;
	std::cout<<"Please Enter Your Nick Name: ";
	fflush(stdout);
	std::cin>>_nick_name;
	std::cout<<std::endl;
	std::cout<<"Please Enter Your School: ";
	fflush(stdout);
	std::cin>>_school;
	std::cout<<std::endl;
	in_data.set_nick_name(_nick_name);
	in_data.set_school(_school);

	pthread_t header, output, list, input;
	pthread_create(&header, NULL, header_run, NULL);
	pthread_create(&output, NULL, output_run, NULL);
	pthread_create(&list, NULL, list_run, NULL);
	pthread_create(&input, NULL, input_run, NULL);

	pthread_join(header, NULL);
	pthread_join(output, NULL);
	pthread_join(list, NULL);
	pthread_join(input, NULL);

	return 0;
}



