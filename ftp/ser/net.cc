#pragma once
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "cmd.cc"
#include "threadpool.hpp"
const int BUFFER_MAXSIZE = 1024;
class Net{
    int server_fd;
public:
    int socket1(int domain, int type, int protocol)
    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0)
        {
            perror("socket");
            exit(1);
        }
        return server_fd;
    }
    void binlis(int fd, const sockaddr *addr, socklen_t len,struct sockaddr_in server_addr,int PORT){
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            perror("bind");
            close(server_fd);
            exit(1);
        }
        if (listen(server_fd, 100) < 0)
        {
            perror("listen");
            close(server_fd);
            exit(1);
        }
    }
    int accept1(int fd, sockaddr *__restrict__ __addr, struct sockaddr_in client_addr)
    {
        socklen_t cli_sock_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &cli_sock_len);
        if (client_fd < 0)
        {
            perror("accept");
            exit(1);
        }
        return client_fd;
    }
    void handclient(int client_fd, int data_fd)
    {
        secv person;
        cmd sendorder;
        char buffer[BUFFER_MAXSIZE];
        person.recv1(client_fd, buffer, BUFFER_MAXSIZE, 0);
        std::cout << "Received command: " << buffer << std::endl;
        std::string order((char *)buffer);
        sendorder.handcmd(order, client_fd,data_fd);
    }
};