#pragma once
#include <iostream>
#include <filesystem>
#include <sys/socket.h>
#include "ispath.cc"
class cmd
{
    friend class pathtask;
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
    void sendResponse(int code, const std::string &message, int client_fd)
    {
        std::string response = std::to_string(code) + " " + message + "\r\n";
        send(client_fd, response.c_str(), response.size(), 0);
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
            if (chdir(args.c_str()) == -1)
            {
                sendResponse(550, "Failed to change directory.",client_fd); //切换失败
            }
            else
            {
                sendResponse(250, "Directory successfully changed.",client_fd); //成功
            }
        }
    }
};