// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <string.h>
// #include <unistd.h>
// #include <algorithm>
// #define BUFFER_MAXSIZE 1024
// #define SERVER_PORT 12345
// #define SERVER_IP "127.0.0.1"
// int main(){
//     int sockfd;
//     if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
//         perror("socket build error");
//         exit(1);
//     }
//     struct sockaddr_in address;
//     address.sin_family = AF_INET;
//     address.sin_port = htons(12345);
//     if(inet_pton(AF_INET,SERVER_IP,&address.sin_addr.s_addr) <= 0){
//         perror("server address is error");
//         close(sockfd);
//         exit(1);
//     }
//     if(connect(sockfd,(struct sockaddr*)&address,sizeof(address)) < 0){
//         perror("server error");
//         close(sockfd);
//         exit(1);
//     }
//     std::cout << "已连接到服务器" << SERVER_IP << " " << SERVER_PORT << std::endl;
//     fd_set readfds;
//     char send_buffer[BUFFER_MAXSIZE];
//     char recv_buffer[BUFFER_MAXSIZE];

//     while (true)
//     {
//         FD_ZERO(&readfds);
//         FD_SET(STDIN_FILENO, &readfds); // 监控标准输入
//         FD_SET(sockfd, &readfds);
//         int max_fd = std::max(sockfd, STDIN_FILENO);
//         std::cout << "[you]: ";
//         std::cin.getline(send_buffer, BUFFER_MAXSIZE - 1);

//         if (strcmp(send_buffer, "exit") == 0)
//             break;

//         // 发送数据（处理部分发送）
//         int send_bytes = send(sockfd, send_buffer, strlen(send_buffer), 0);
//         if(send_bytes <= 0){
//             perror("send error");
//             goto exit;
//         }
//         std::cout << "已发送 " << send_bytes << "字节" << std::endl;
//         // 接收数据（处理部分接收）
//         int recv_bytes = recv(sockfd, recv_buffer, BUFFER_MAXSIZE, 0);
//         if(recv_bytes < 0){
//             perror("recv error");
//             goto exit;
//         }
//         recv_buffer[recv_bytes] = '\0';
//         std::cout << "服务器响应: " << recv_buffer << std::endl;
//     }

// exit:
//     close(sockfd);
//     std::cout << "客户端已退出" << std::endl;
//     return 0;
// }
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <algorithm>

#define BUFFER_MAXSIZE 1024
#define SERVER_PORT 12345
#define SERVER_IP "127.0.0.1"

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket build error");
        exit(1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &address.sin_addr.s_addr) <= 0)
    {
        perror("server address is error");
        close(sockfd);
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect failed");
        close(sockfd);
        exit(1);
    }

    std::cout << "已连接到服务器 " << SERVER_IP << " 端口 " << SERVER_PORT << std::endl;

    fd_set readfds;
    char send_buffer[BUFFER_MAXSIZE];
    char recv_buffer[BUFFER_MAXSIZE];

    bool running = true;
    while (running)
    {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sockfd, &readfds);
        int max_fd = std::max(sockfd, STDIN_FILENO);

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            perror("select error");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            std::cin.getline(send_buffer, BUFFER_MAXSIZE - 1);
            if (strcmp(send_buffer, "exit") == 0)
                break;

            if (send(sockfd, send_buffer, strlen(send_buffer), 0) <= 0)
            {
                perror("send error");
                break;
            }
        }

        if (FD_ISSET(sockfd, &readfds))
        {
            int recv_bytes = recv(sockfd, recv_buffer, BUFFER_MAXSIZE, 0);
            if (recv_bytes <= 0)
            {
                std::cout << "服务器断开连接" << std::endl;
                break;
            }
            recv_buffer[recv_bytes] = '\0';
            std::cout << "服务器响应: " << recv_buffer << std::endl;
        }
    }

    close(sockfd);
    std::cout << "客户端已退出" << std::endl;
    return 0;
}
