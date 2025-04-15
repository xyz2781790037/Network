#pragma once
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "cmd.cc"
#include "respond_fd.hpp"
class secv
{
public:
    void recv1(int &fd,void* buf,size_t n,int flags){
        int recv_cmd = recv(fd, buf, n, flags);
        if(recv_cmd <= 0){
            perror("recvcmd");
            sendResponse(421, "Control connection error", fd);
            close(fd);
        }
    }
    int send1(int &read_fd, int &write_fd, size_t n)
    {
        char buffer[1024];
        int read_bytes;

        while ((read_bytes = read(read_fd, buffer, n)) > 0){
            if (write(write_fd, buffer, read_bytes) != read_bytes)
            {
                perror("write");
                close(write_fd);
                sendResponse(550, "Write error during file transfer.\r\n", read_fd);
                return -1;
            }
        }
        if (read_bytes == 0){
            // 文件读完了
            sendResponse(226, "Transfer complete.\r\n",read_fd);
            return 0;
        }
        else{
            // 读出错
            sendResponse(451, "Read error during file transfer.\r\n",read_fd);
            return -1;
        }
    }
};