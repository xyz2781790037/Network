#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
#define SERVER_IP "127.0.0.1" // 服务器IP地址
#define SERVER_PORT 8088      // 服务器端口
#define BUFFER_SIZE 1024      // 缓冲区大小
pthread_mutex_t mutex;
struct ThreadArgs
{
    int sockfd;
};
void *receive(void *arg)
{
    pthread_mutex_lock(&mutex);
    ThreadArgs *a = static_cast<ThreadArgs *>(arg);
    std::string buffer;
    while (1)
    {
        ssize_t recv_bytes = recv(a->sockfd, (void *)buffer.c_str(), BUFFER_SIZE, 0);
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
        std::cout << buffer << std::endl;
        buffer.clear();
    }
    pthread_mutex_unlock(&mutex);
    return nullptr;
}
int main()
{
    pthread_mutex_init(&mutex, nullptr);
    int sockfd;
    struct sockaddr_in server_addr;
    std::string buffer;
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
    struct ThreadArgs Arg = {sockfd};
    pthread_t pth;
    pthread_create(&pth, nullptr, receive, &Arg);
    while (1)
    {
        std::cout << "请输入要发送的消息 (输入exit退出):";
        getline(std::cin, buffer);
        if (buffer == "exit")
        {
            break;
        }
        ssize_t sent_bytes = send(sockfd, buffer.c_str(), buffer.size(), 0);
        if (sent_bytes < 0)
        {
            perror("send error");
            break;
        }
        std::cout << "已发送 " << sent_bytes << "字节" << std::endl;
    }
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    pthread_join(pth, nullptr);
    std::cout << "连接已关闭" << std::endl;
    return 0;
}