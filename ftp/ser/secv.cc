#pragma once
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "cmd.cc"
#include "respond_fd.hpp"
class secv
{
    // friend class cmd;
    ssize_t readn(int fd, void *buffer, size_t n)
    {
        ssize_t numRead;
        size_t toRead;
        char *buf;
        buf = (char*)buffer;
        for (toRead = 0; toRead < n;)
        {
            numRead = read(fd, buf, n - toRead);
            if (numRead == 0)
            {
                return toRead;
            }
            if (numRead == -1)
            {
                if (errno == EINTR)
                { // 系统调用被信号中断，重试
                    continue;
                }
                else
                {
                    return -1;
                }
            }
            toRead += numRead;
            buf += numRead; // 把指针往后移，跳过已经读取的部分。
        }
        return toRead;
    }
    ssize_t writen(int fd, const void *buffer, size_t n)
    {
        ssize_t numWritten;
        size_t toWritten;
        const char *buf;
        buf = (char *)buffer;
        for (toWritten = 0; toWritten < n;)
        {
            numWritten = write(fd, buf, n - toWritten);
            if (numWritten <= 0)
            {
                if (numWritten == -1 && errno == EINTR)
                {
                    continue;
                }
                else
                {
                    return -1;
                }
            }
            toWritten += numWritten;
            buf += numWritten;
        }
        return toWritten;
    }

public:
    void recv1(int &fd,void* buf,size_t n,int flags){
        int recv_cmd = recv(fd, buf, n, flags);
        if(recv_cmd < 0){
            perror("recvcmd");
            sendResponse(421, "Control connection error", fd);
            close(fd);
        }
    }
    int send1(int &read_fd, int &write_fd, size_t n)
    {
        char buffer[1024];
        int read_bytes;

        while ((read_bytes = readn(read_fd, buffer, n)) > 0){
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