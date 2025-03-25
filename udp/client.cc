#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include "th_pool.hpp"
#define SERVER_IP "127.0.0.1" // 服务器IP地址
#define SERVER_PORT 8080      // 服务器端口
#define BUFFER_SIZE 1024      // 缓冲区大小
bool runflag = true;
struct ThreadArgs
{
    int sockfd;
    struct sockaddr_in server;
};
void *receive(void *arg)
{
    char buffer[BUFFER_SIZE];
    ThreadArgs *sock = static_cast<ThreadArgs *>(arg);
    socklen_t addr_len = sizeof(sock->server);
    while (runflag)
    {
        ssize_t recv_len = recvfrom(sock->sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&sock->server, &addr_len);
        buffer[recv_len] = '\0';
        if (recv_len > 0)
        {
            std::cout << "收到服务器回复: " << buffer << std::endl;
        }
    }
    return nullptr;
}
int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    std::string buffer;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket build failed");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    std::cout << "UDP 客户端已启动，输入消息并按 Enter 发送..." << std::endl;
    ThreadArgs th = {sockfd, server_addr};
    pool po;
    po.enqueue(receive, &th);
    while (1)
    {
        std::cout << "输入消息: ";
        getline(std::cin, buffer);
        sendto(sockfd, buffer.c_str(), buffer.size(), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (buffer == "exit")
        {
            std::cout << "客户端退出..." << std::endl;
            break;
        }
    }
    close(sockfd);
    return 0;
}