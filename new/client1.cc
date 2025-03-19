#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "th_pool.hpp"
#define SERVER_IP "127.0.0.1" // 服务器IP地址
#define SERVER_PORT 8080      // 服务器端口
#define BUFFER_SIZE 1024      // 缓冲区大小
void *receive(void *arg)
{
    int *sock = static_cast<int *>(arg);
    std::string buffer;
    while (1)
    {
        ssize_t recv_bytes = recv(*sock, (void *)buffer.c_str(), BUFFER_SIZE, 0);
        if (recv_bytes < 0)
        {
            perror("receive failed");
            break;
        }
        else if (recv_bytes == 0)
        {
            std::cout << "服务器已关闭连接" << std::endl;
            break;
        }
        std::cout << "收到响应" << recv_bytes << "字节" << std::endl;
        printf("%s", buffer.c_str());
        buffer.clear();
    }
    delete sock;
    return nullptr;
}
void *sendd(void *arg)
{
    int *sock = static_cast<int *>(arg);
    std::string buffer;
    while (1)
    {
        std::cout << "请输入要发送的消息 (输入exit退出):";
        getline(std::cin, buffer);
        if (buffer == "exit")
        {
            break;
        }
        ssize_t sent_bytes = send(*sock, buffer.c_str(), buffer.size(), 0);
        if (sent_bytes < 0)
        {
            perror("send error");
            break;
        }
        std::cout << "已发送 " << sent_bytes << "字节" << std::endl;
    }
    delete sock;
    return nullptr;
}
int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket build failed");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("server address is error");
        exit(1);
    }
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect error");
        exit(1);
    }
    std::cout << "已连接到服务器" << SERVER_IP << " " << SERVER_PORT << std::endl;
    pool po;
    while (1)
    {
        po.enqueue(receive, &sockfd);
        po.enqueue(sendd, &sockfd);
    }
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    po.end_thread();
    return 0;
}