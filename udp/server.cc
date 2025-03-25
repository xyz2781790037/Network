#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include 
#define PORT 8080
int main()
{
    int sockfd;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    int addrlen = sizeof(address);
    if (sockfd = socket(AF_INET, SOCK_DGRAM, 0) == 0)
    {
        perror("socket build error");
        exit(1);
    }
    if ((bind(sockfd, (struct sockaddr *)&address, addrlen)) < 0)
    {
        perror("bind error");
        exit(1);
    }
    if(listen(sockfd,5) < 0){
        perror("listen error");
        exit(1);
    }
    std::cout << "服务器正在监听端口" << PORT << std::endl;
    return 0;
}