#include <iostream>
#include "th_pool.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_SIZE 1024
void*handclient(void*arg){
    int *sock = static_cast<int *>(arg);
    int valread;
    std::string buffer;
    buffer.resize(BUFFER_SIZE);
    while((valread = read(*sock,(void*)buffer.c_str(),BUFFER_SIZE)) > 0){
        std::cout << "收到" << valread << "字节数据" << std::endl;
        std::cout << buffer << std::endl;
        sendall()
    }
}
int main()
{
    int sockfd, new_socket;
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
    if((bind(sockfd,(struct sockaddr*)&address,sizeof(address))) < 0){
        perror("bind error");
        exit(1);
    }
    if(listen(sockfd,4) < 0){
        perror("listen error");
        exit(1);
    }
    std::cout << "服务器正在监听端口" << PORT << std::endl;
    pool po;
    while(1){
        if((new_socket = accept(sockfd,(struct sockaddr*)&address,(socklen_t*)&addrlen)) < 0){
            perror("accept失败");
            continue;
        }
        std::cout << "新连接来自" << inet_ntoa(address.sin_addr) << ntohs(address.sin_port) << std::endl;
        po.enqueue(handclient, &new_socket);
    }
    return 0;
}