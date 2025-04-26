#pragma once
#include <iostream>
#include <atomic>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "resv.hpp"
class handstr
{
    resv sre;
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
    void storhelper(std::string input, int fd, int data_fd)
    {
        size_t pathpos = input.find_first_of(' ');
        size_t pathpos2 = input.find_last_of(' ');
        std::string tmp1 = input.substr(0, pathpos);
        std::string tmp2 = input.substr(pathpos2);
        tmp1 += tmp2;
        ssize_t send_bytes = sre.send1(fd, tmp1, tmp1.size(), 0,"send1");
        std::string buffer;
        ssize_t recv_bytes = sre.recv1(fd, buffer, 1024, 0,"recv1");
        std::string filename = input.substr(pathpos + 1, pathpos2 - pathpos - 1);

        stor(filename, data_fd);
    }
    void stor(std::string filename, int data_fd)
    {
        char filname[filename.size()];
        strcpy(filname, filename.c_str());
        int file_fd = open(filname, O_RDONLY, 0644);
        if (file_fd < 0)
        {
            perror("open");
            return;
        }
        std::cout << "filename :" << filname << std::endl;
        struct stat st;
        fstat(file_fd, &st);
        int bytes = sendfile(data_fd, file_fd, NULL, st.st_size);
        // shutdown(data_fd, SHUT_WR);
        if(bytes == -1){
            perror("sendfile");
            if (errno == EPIPE || errno == ECONNRESET)
            {
                std::cerr << "Server closed connection early" << std::endl;
                return;
            }
        }
        std::string buffer;
        ssize_t recv_bytes = sre.recv1(data_fd, buffer, 1024, 0,"recv sendfile");
        
    }
    void cdsend(int &fd, std::string input)
    {
        int send_bytes = sre.send1(fd, input, input.size(), 0,"send");
        std::string buffer;
        ssize_t recv_bytes = sre.recv1(fd, buffer, 1024, 0,"recv");
    }
    void list(std::string input,int &fd,int &data_fd){
        int send_bytes = sre.send1(fd, input, input.size(), 0, "send1");
        std::string buffer;
        int recv_bytes = sre.recv1(fd, buffer,1024,0,"recv list");
        recvlist(data_fd);
    }
    void recvlist(int &data_fd){
        char buffer[1024];
        ssize_t bytes;
        while ((bytes = recv(data_fd, buffer, sizeof(buffer) - 1, 0)) > 0)
        {
            buffer[bytes] = '\0';
            std::cout << buffer;
        }
        if (bytes == 0)
        {
            sre.send1(data_fd, "send ok", sizeof("send ok"), 0, "send ok");
        }
        else if (bytes < 0)
        {
            perror("接收失败");
        }
    }
    void retrhelper(std::string input,int &fd,int &dfd){
        size_t filepos = input.find_last_of(' ');
        std::string buffers = input.substr(0, filepos);
        std::string path = input.substr(filepos + 1);
        ssize_t send_bytes = sre.send1(fd, buffers, buffers.size(), 0, "send1");
        std::cout << buffers << std::endl;
        std::string str;
        ssize_t recv_bytes = sre.recv1(fd, str, 1024, 0, "recv1");
        std::cout << str << std::endl;
        retr(path,dfd);
    }
    void retr(std::string args,int&dfd){
        size_t filepos = args.find_last_of('/');
        std::string filename = args.substr(filepos + 1);
        std::string path = args.substr(0, filepos);
        chdir(path.c_str());
        char filname[filename.size()];
        strcpy(filname, filename.c_str());
        int file_fd = open(filname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file_fd < 0)
        {
            perror("open");
            return;
        }
        sre.recvfile(dfd, file_fd, 1024);
    }

public:
    void inputseg(std::string &input, int &fd, int &data_fd)
    {
        input = segstrspace(input);
        size_t pathpos = input.find_first_of(' ');
        std::string order;
        if (pathpos < 0)
        {
            order = input;
        }
        else
        {
            order = input.substr(0, pathpos);
        }
        if (order == "PASV")
        {
        }
        else if (order == "LIST")
        {
            list(input, fd, data_fd);
        }
        else if (order == "STOR")
        {
            storhelper(input, fd, data_fd);
        }
        else if (order == "RETR")
        {
            retrhelper(input, fd, data_fd);
        }
        else if (order == "MKD")
        {
            cdsend(fd, input);
        }
        else
        {
            // std::cout << "error input" << std::endl;
            // sendResponse(500, "invalid commend", client_fd);
        }
    }
};