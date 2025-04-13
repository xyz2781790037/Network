#pragma once
#include <iostream>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
class handstr
{
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
    void storhelper(std::string input,int fd){
        size_t pathpos = input.find_first_of(' ');
        size_t pathpos2 = input.find_last_of(' ');
        std::string tmp1 = input.substr(0, pathpos);
        std::string tmp2 = input.substr(pathpos2);
        tmp1 += tmp2;
        ssize_t send_bytes = send(fd, tmp1.c_str(), tmp1.size(), 0);
        if (send_bytes < 0)
        {
            perror("send");
            return;
        }
        char buffer[1024];
        ssize_t recv_bytes = recv(fd, buffer, 1024, 0);
        if (recv_bytes < 0)
        {
            perror("recv");
            return;
        }
        std::string filename = input.substr(pathpos + 1, pathpos2 - pathpos - 1);
        stor(filename, fd);
    }

public:
    void inputseg(std::string &input, int&fd)
    {
        input = segstrspace(input);
        size_t pathpos = input.find_first_of(' ');
        std::string order, args;
        if(pathpos < 0){

        }
        else{
            order = input.substr(0, pathpos);
            args = input.substr(pathpos + 1);
        }
        if (order == "PASV")
        {
        }
        else if (order == "ACTION")
        {
        }
        else if (order == "LIST")
        {
        }
        else if (order == "STOR")
        {
            storhelper(input,fd);
        }
        else if (order == "RETR")
        {
        }
        else if (order == "MKD")
        {
            // if (!handp.createDir(args))
            // {
            //     perror("mkdir");
            // }
        }
        else if (order == "CWD")
        {
            // cwd(args, client_fd);
        }
        else
        {
            // std::cout << "error input" << std::endl;
            // sendResponse(500, "invalid commend", client_fd);
        }
    }
    void stor(std::string filename, int client_fd)
    {

        int file_fd = open(filename.c_str(), O_RDONLY);
        if (file_fd < 0)
        {
            perror("open");
            return;
        }
        struct stat st;
        fstat(file_fd, &st);
        sendfile(client_fd, file_fd, NULL, st.st_size);
    }
};