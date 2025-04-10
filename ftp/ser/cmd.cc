#pragma once
#include <iostream>
#include <filesystem>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "ispath.cc"
#include "respond_fd.hpp"
#include "secv.cc"
class secv;
class cmd
{
    secv* refile;
    pathtask handp;
    std::string segstrspace(std::string &order, int count = 0)
    {
        while (count < order.size())
        {
            if ((order[count] == ' ' && count == 0) || (order[count] == ' ' && count == order.size() - 1) || (count + 1 < order.size() && order[count] == ' ' && order[count + 1] == ' '))
            {
                order.erase(count, 1);
            }
            else
            {
                count++;
            }
        }
        return order;
    }
    void stor(int fd,std::string args){

        size_t filepos = args.find_last_of('/');
        std::string filename = args.substr(filepos + 1);
        std::string path = args.substr(0, filepos);
        cwd(path, fd);
        int file_fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file_fd < 0)
        {
            perror("open");
            sendResponse(550, "Failed to open file for writing.\r\n",fd);
            return;
        }
        refile->send1(fd, file_fd,1024);
    }
    void cwd(std::string args,int client_fd){
        if (chdir(args.c_str()) == -1)
        {
            sendResponse(550, "Failed to change directory.", client_fd); // 切换失败
        }
        else
        {
            sendResponse(250, "Directory successfully changed.", client_fd); // 成功
        }
    }
public:
    void handcmd(std::string orders,int client_fd){
        orders = segstrspace(orders);
        ssize_t cmdspace = orders.find_first_of(' ');
        std::string order = orders.substr(0, cmdspace);
        std::string args = orders.substr(cmdspace + 1);
        if (order == "PASV"){
        }
        else if(order == "ACTION"){
            
        }
        else if (order == "LIST"){

        }
        else if (order == "STOR"){
            stor(client_fd, args);
        }
        else if(order == "RETR"){

        }
        else if(order == "MKD"){
            if(!handp.createDir(args)){
                perror("mkdir");
            }
        }
        else if (order == "CWD")
        {
            cwd(args,client_fd);
        }
    }
};