#ifndef _HTTPD_HPP_H_
#define _HTTPD_HPP_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.hpp"

#define HTTP_ROOT "wwwroot"
#define MAIN_PAGE "index.html"

typedef enum{
    OK=200,
    NOT_FOUND=404,
}status_t;

typedef std::unordered_map<std::string, std::string> Param_t;

Param_t g_text_type{
    {".html", "text/html\n"},
    {".js", "application/x-javascript\n"},
    {".css", "text/html\n"},
};

class Request{
    public:
        std::string line;
        std::string method;
        std::string url;
        std::string http_version;
        Param_t header_body;

        std::string path;
        std::string suffix;
        std::string query_string;
        bool cgi;
        int content_length;
        int file_size;
    public:
        Request():cgi(false)
        {}
        ~Request()
        {}
};

class Response{
    public:
        status_t code;
        std::string header;
        std::string header_body;

        int content_length;
    public:
        Response():code(OK)
        {}
        ~Response()
        {}
};

class Context{
    public:
        int sock;
        Request http_request;
        Response http_response;
    public:
        Context(int sock_):sock(sock_)
        {}
        ~Context()
        {}
};

class StringUtil{
    public:
        static bool MakeKV(Param_t &param_map, std::string &s)
        {
            std::string sep_ = ": ";
            std::size_t pos = s.find(sep_);
            if(std::string::npos == pos){
                return false;
            }

            std::string k_ = s.substr(0, pos);
            std::string v_ = s.substr(pos+sep_.size());
            param_map.insert(std::make_pair(k_, v_));
            return true;
        }
};

class ProtocolUtil{
    public:
        static int GetLine(Context *&ct)
        {
            int &sock_ = ct->sock;
            char c = 'x';
            std::string &line_ = (ct->http_request).line;
            line_ = "";
            while(c != '\n'){
                ssize_t s = recv(sock_, &c, 1, 0);
                if(s > 0){
                    if(c == '\r'){
                        recv(sock_, &c, 1, MSG_PEEK);
                        if(c == '\n'){
                            recv(sock_, &c, 1, 0);
                        }
                        else{
                            c = '\n';
                        }
                    }
                    line_.push_back(c);
                }
                else{
                    return line_.size();
                }
            }
        }
        static void RequestLineParse(Context *&ct)
        {
            std::string &method_ = (ct->http_request).method;
            std::string &url_ = (ct->http_request).url;
            std::string &http_version_ = (ct->http_request).http_version;
            std::stringstream ss((ct->http_request).line);
            ss >> method_ >> url_ >> http_version_;
        }
        static bool CheckMethodVaild(Context *&ct)
        {
            bool &cgi_ = (ct->http_request).cgi;
            std::string &method_ = (ct->http_request).method;
            cgi_ = false;
            if((cgi_ = (strcasecmp(method_.c_str(), "POST") == 0)) || strcasecmp(method_.c_str(), "GET") == 0){
                return true;
            }
            return false;
        }
        static void RequestUrlParse(Context *&ct)
        {
            std::string &path_ = (ct->http_request).path;
            std::string &query_string_ = (ct->http_request).query_string;
            std::string &url_ = (ct->http_request).url;
            bool &cgi_ = (ct->http_request).cgi;
        
            int pos = url_.find("?");
            if(std::string::npos != pos){
                query_string_ = url_.substr(pos+1);
                url_ = url_.substr(0, pos);
                cgi_ = true;
            }
            path_ = HTTP_ROOT;
            path_ += url_;
            if(path_[path_.size() - 1 ] == '/'){
                path_ += MAIN_PAGE;
            }
        }
        static void HeaderBodyParse(Context *&ct, std::string &body)
        {
            Param_t &header_body_ = (ct->http_request).header_body;
            std::size_t pos = 0;
            std::size_t start = 0;
            while( start < body.size() && std::string::npos != (pos = body.find('\n', start)) ){
                std::string sub_ = body.substr(start, pos - start);
                StringUtil::MakeKV(header_body_, sub_);
                start = pos + 1;
            }
            std::string &method_ = (ct->http_request).method;
            if(strcasecmp(method_.c_str(), "POST") == 0){
                int &content_length_ = (ct->http_request).content_length;
                std::string cl_ = header_body_["Content-Length"];
                if(!cl_.empty()){
                    content_length_ = -1;
                }else{
                    std::stringstream ss;
                    ss << cl_;
                    ss >> content_length_;
                }
            }
        }
        static bool CheckPathVaild(Context *&ct)
        {
            std::string &path_ = (ct->http_request).path;
            std::string &suffix_ = (ct->http_request).suffix;
            int &size_ = (ct->http_request).file_size;
            bool &cgi_ = (ct->http_request).cgi;
            struct stat st;

            LOG(INFO, path_);
            if(stat(path_.c_str(), &st)){
                return false;
            }
            else{
                if(S_ISDIR(st.st_mode)){
                    path_ += "/";
                    path_ += MAIN_PAGE;
                }else if((st.st_mode & S_IXUSR) ||\
                        (st.st_mode & S_IXGRP) ||\
                        (st.st_mode & S_IXOTH)){
                    cgi_ = true;
                }else{
                    //DO NOTHING
                }
            }
            size_ = st.st_size;
            std::size_t pos_ = path_.find_last_of('.');
            if(pos_ != std::string::npos){
                suffix_ = path_.substr(pos_); //.css .html .js
            }else{
                suffix_ = "";
            }

            return true;
        }
        static void BuildResponse(Context *&ct)
        {
            std::string &path_ = (ct->http_request).path;
            std::string &header_ = (ct->http_response).header;
            std::string &suffix_ = (ct->http_request).suffix;
            std::string &header_body_ = (ct->http_response).header_body;
            int &size_ = (ct->http_request).file_size;
            status_t &code_ = (ct->http_response).code;

            std::stringstream ss;
            ss << code_;

            header_ = "HTTP/1.0 ";
            header_ += ss.str() + " OK\n";
            header_body_ = "Content-Type: ";
            header_body_ += g_text_type[suffix_];
            header_body_ += "Content-Length: ";
            ss.str("");
            ss << size_<< "\n\n";
            header_body_ += ss.str();
        }
        static void ReadAll(Context *&ct)
        {
            int &sock_ = ct->sock;
            Request &rq_ = ct->http_request;
            std::string &line_ = rq_.line;
            std::string tmp_;
            line_ = "";
            while(line_ != "\n"){
                GetLine(ct);
                tmp_ += line_;
            }
            HeaderBodyParse(ct, tmp_);
            int len_ = (ct->http_request).content_length;
            std::string &method_ = (ct->http_request).method;
            std::string &query_string_ = (ct->http_request).query_string;
            char ch;
            ssize_t s_;
            while(strcasecmp(method_.c_str(), "POST") == 0 && len_ > 0){
                s_ = recv(sock_, &ch, 1, 0);
                if(s_ <= 0){
                    break;
                }
                len_--;
                query_string_.push_back(ch);
            }
        }
};

class Entry{
    public:
        static void ProcessCgi(Context *&ct)
        {
            int input[2];
            int output[2];
            pipe(input);
            pipe(output);

            int &sock_ = ct->sock;
            std::string &arg = (ct->http_request).query_string;
            std::stringstream ss;
            ss << arg.size();
            std::string size_str_ = ss.str();

            LOG(DEBUG, arg);

            pid_t id = fork();
            if(id < 0){
            }
            else if(id == 0){
                std::string &path_ = (ct->http_request).path;
                LOG(DEBUG, path_);

                close(input[1]); // child read
                close(output[0]); //child write

                dup2(input[0], 0);
                dup2(output[1], 1);

                std::string content_length_env_ = "Content_Length=";
                content_length_env_ += size_str_;
                putenv((char*)content_length_env_.c_str());
                execl(path_.c_str(), path_.c_str(), NULL);
                exit(1);
            }
            else{//father
                close(input[0]);
                close(output[1]);

                ssize_t s = 0;
                int num = arg.size();
                int total = 0;
                const char *str = arg.c_str();
                while( total != num && (s = write(input[1], str + total, num - total)) > 0 ){
                    total += s;
                }

                char c;
                while(read(output[0], &c, 1) > 0){
                    send(sock_, &c, 1, 0);
                }

                waitpid(id, NULL, 0);
                close(input[1]);
                close(output[0]);
            }
        }
        static void ProcessNonCgi(Context *&ct)
        {
            int &sock_ = ct->sock;
            int &size_ = (ct->http_request).file_size;
            status_t &code_ = (ct->http_response).code;

            std::string &header_ = (ct->http_response).header;
            std::string &header_body_ = (ct->http_response).header_body;
            std::string &path_ = (ct->http_request).path;
            int fd_ =  open(path_.c_str(), O_RDONLY);
            if(fd_ < 0){
                code_ = NOT_FOUND;
                return;
            }

            ProtocolUtil::BuildResponse(ct);

            send(sock_, header_.c_str(), header_.size(), 0);
            send(sock_, header_body_.c_str(), header_body_.size(), 0);

            sendfile(sock_, fd_, NULL, size_);
            close(fd_);
        }
        static void HttpProcess(Context *&ct)
        {
            LOG(INFO, "request process begin!");
            bool &cgi_ = (ct->http_request).cgi;
            ProtocolUtil::ReadAll(ct);
            if(cgi_){
                LOG(INFO, "request handler througt cgi");
                ProcessCgi(ct);
            }else{
                LOG(INFO, "request handler througt non_cgi");
                ProcessNonCgi(ct);
            }
        }
        static void HandlerRequest(int sock)
        {
            Context *ct = new Context(sock);
            status_t &code_ = (ct->http_response).code;

            ProtocolUtil::GetLine(ct);
            ProtocolUtil::RequestLineParse(ct);
            if(!ProtocolUtil::CheckMethodVaild(ct)){
                LOG(ERROR, "request method error!");
                code_ = NOT_FOUND;
                goto end;
            }
            ProtocolUtil::RequestUrlParse(ct);
            if(!ProtocolUtil::CheckPathVaild(ct)){
                LOG(ERROR, "request path error!");
                code_ = NOT_FOUND;
                goto end;
            }
            HttpProcess(ct);
        end:
            close(ct->sock);
            delete ct;
        }

};
#endif

