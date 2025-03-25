#include <iostream>
#include "th_pool.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#define PORT 8080
#define BUFFER_SIZE 1024
struct ThreadArgs
{
    int read_sock;  // 要读取的socket
    int write_sock; // 要写入的socket
};
void *handclient(void *arg)
{
    ThreadArgs *sock = static_cast<ThreadArgs *>(arg);
    std::string buffer;
    while (1)
    {
        ssize_t valread = recv(sock->read_sock, (void*)buffer.c_str(), BUFFER_SIZE, 0);
        if (valread <= 0)
            break;

        std::cout << "转发 " << valread << "字节数据" << std::endl;
        send(sock->write_sock, buffer.c_str(), valread, 0);
    }
    close(sock->read_sock);
    close(sock->write_sock);
    delete sock; // 释放动态分配的内存
    return nullptr;
}
int main()
{
    int sockfd, sock1, sock2;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket build error");
        exit(1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if ((bind(sockfd, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        perror("bind error");
        exit(1);
    }
    if (listen(sockfd, 4) < 0)
    {
        perror("listen error");
        exit(1);
    }
    std::cout << "服务器正在监听端口" << PORT << std::endl;
    pool po;
    while (1)
    {
        if ((sock1 = accept(sockfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept失败");
            continue;
        }
        std::cout << "新连接来自" << inet_ntoa(address.sin_addr) << ntohs(address.sin_port) << std::endl;
        if ((sock2 = accept(sockfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept失败");
            close(sock1);
            continue;
        }
        std::cout << "新连接来自" << inet_ntoa(address.sin_addr) << ntohs(address.sin_port) << std::endl;
        ThreadArgs *args1 = new ThreadArgs{sock1, sock2};
        ThreadArgs *args2 = new ThreadArgs{sock2, sock1};
        po.enqueue(handclient, args1);
        po.enqueue(handclient, args2);
    }
    po.end_thread();
    return 0;
}