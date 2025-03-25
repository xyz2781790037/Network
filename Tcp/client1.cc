#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "th_pool.hpp"
#define SERVER_IP "127.0.0.1" // 服务器IP地址
#define SERVER_PORT 8080      // 服务器端口
#define BUFFER_SIZE 1024      // 缓冲区大小
bool runflag = true;
void *receive(void *arg)
{
    int *sock = static_cast<int *>(arg);
    char buffer[BUFFER_SIZE];
    while (runflag)
    {
        ssize_t recv_bytes = recv(*sock, buffer, BUFFER_SIZE, 0);
        if (recv_bytes < 0)
        {
            perror("receive failed");
            runflag = false;
            break;
        }
        else if (recv_bytes == 0)
        {
            std::cout << "服务器已关闭连接" << std::endl;
            runflag = false;
            break;
        }
        buffer[recv_bytes] = '\0';
        std::cout << "\n[he]:" << buffer << std::endl;
    }
    return nullptr;
}
void *sendd(void *arg)
{
    int *sock = static_cast<int *>(arg);
    std::string buffer;
    while (runflag)
    {
        std::cout << "[you]:";
        getline(std::cin, buffer);
        if (buffer == "exit")
        {
            runflag = false;
            break;
        }
        ssize_t sent_bytes = send(*sock, buffer.c_str(), buffer.size(), 0);
        if (sent_bytes < 0)
        {
            perror("send error");
            runflag = false;
            break;
        }
        std::cout << "已发送 " << sent_bytes << "字节" << std::endl;
    }
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
    po.enqueue(receive, &sockfd);
    po.enqueue(sendd, &sockfd);
    while (runflag)
    {
        sleep(1);
    }
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    po.end_thread();
    std::cout << "客户端已退出" << std::endl;
    return 0;
}