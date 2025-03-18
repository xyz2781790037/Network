#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define PORT 8088        // 监听的端口号
#define BUFFER_SIZE 1024 // 缓冲区大小
pthread_mutex_t mutex;
void *handleClient(void *arg)
{
    pthread_mutex_lock(&mutex);
    int *a = static_cast<int *>(arg);
    int valread;
    std::string buffer;
    buffer.resize(BUFFER_SIZE);
    while ((valread = read(*a, (void *)buffer.c_str(), BUFFER_SIZE)) > 0)
    {
        std::cout << "收到" << valread << "字节数据" << std::endl;
        std::cout << buffer << std::endl;
        send(*a, buffer.c_str(), valread, 0);
        buffer.clear();
        buffer.resize(BUFFER_SIZE);
    }
    close(*a);
    std::cout << "客户端连接关闭" << std::endl;
    pthread_mutex_unlock(&mutex);
    return nullptr;
}
int main()
{
    pthread_mutex_init(&mutex, nullptr);
    int sockfd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    std::string buffer;
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
    pthread_t pth;
    while (1)
    {
        if ((new_socket = accept(sockfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept失败");
            continue;
        }
        std::cout << "新连接来自" << inet_ntoa(address.sin_addr) << ntohs(address.sin_port) << std::endl;
        pthread_create(&pth, nullptr, handleClient, &new_socket);
        // int valread;
        // buffer.resize(BUFFER_SIZE);
        // while ((valread = read(new_socket, (void *)buffer.c_str(), BUFFER_SIZE)) > 0)
        // {
        //     std::cout << "收到" << valread << "字节数据" << std::endl;
        //     std::cout << buffer << std::endl;
        //     send(new_socket, buffer.c_str(), valread, 0);
        //     buffer.clear();
        //     buffer.resize(BUFFER_SIZE);
        // }
        // close(new_socket);
    }
    return 0;
}