#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "th_pool.hpp"
#define PORT 8080
#define BUFFER_SIZE 1024
struct sockaddr_in client1, client2; // 存储两个客户端的地址
socklen_t client1_len = sizeof(client1);
socklen_t client2_len = sizeof(client2);
int main()
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket build error");
        exit(1);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    int addrlen = sizeof(address);
    if ((bind(sockfd, (struct sockaddr *)&address, addrlen)) < 0)
    {
        perror("bind error");
        close(sockfd);
        exit(1);
    }
    std::cout << "UDP 服务器启动，等待客户端连接..." << std::endl;
    std::string buffer;
    pool po;
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        socklen_t recv_len = recvfrom(sockfd, (void *)buffer.c_str(), BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0)
        {
            perror("recvfrom failed");
            continue;
        }
        buffer[recv_len] = '\0';
        std::cout << "收到数据: " << buffer << " 来自 " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
        if (client1.sin_port == 0)
        {
            client1 = client_addr;
            std::cout << "客户端1连接:" << inet_ntoa(client1.sin_addr) << ":" << ntohs(client1.sin_port) << std::endl;
        }
        else if (client2.sin_port == 0 && (client_addr.sin_port != client1.sin_port || client_addr.sin_addr.s_addr != client1.sin_addr.s_addr))
        {
            client2 = client_addr;
            std::cout << "客户端1连接:" << inet_ntoa(client2.sin_addr) << ":" << ntohs(client2.sin_port) << std::endl;
        }
        if (client1.sin_port != 0 && client2.sin_port != 0)
        {
            struct sockaddr_in target = (client_addr.sin_port == client1.sin_port && client_addr.sin_addr.s_addr == client1.sin_addr.s_addr) ? client2 : client1;
            sendto(sockfd, buffer.c_str(), recv_len, 0, (struct sockaddr *)&target, sizeof(target));
            std::cout << "转发给 " << inet_ntoa(target.sin_addr) << ":" << ntohs(target.sin_port) << std::endl;
        }
    }
    close(sockfd);
    return 0;
}