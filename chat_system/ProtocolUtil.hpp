#ifndef _PROTOCOL_UTIL_HPP_
#define _PROTOCOL_UTIL_HPP_

#include <iostream>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>

#define LOGIN 0x01
#define REGISTER 0x02
#define REGISTER_REPLY 0x04
#define NORMAL 0x08

//定长报文，大小256
typedef struct Login{
    char type;
    char id[16]; //QQ
    char passwd[32];
    char pad[207];
}login_t;

typedef struct Register{
    char type;
    char name[32];
    char passwd[32];
    char school[64];
    char telephone[16];
    char pad[111];
}register_t;

typedef struct RegisterReply{
    char type;
    char id[16]; //QQ
    char pad[239];
}resigter_reply_t;

typedef struct Message{
    char type;
    char message[255];
}message_t;

#endif







