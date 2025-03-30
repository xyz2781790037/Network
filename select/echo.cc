#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <unistd.h>
#include <string.h>
#define PORT 12345
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htonl(12345);
    if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
        perror("bind failed");
        exit(1);
    }
    if(listen(sockfd,10) < 0){
        perror("listen failed");
        exit(1);
    }
    std::cout << "正在监听窗口：12345" << std::endl;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    int max_fd = sockfd;
    std::vector<int> client_sockets(MAX_CLIENTS, 0);
    int client_fd;
    while (true)
    {
        fd_set temp_readfds = readfds;
        int acticity = select(max_fd + 1, &temp_readfds, nullptr, nullptr, nullptr);
        if (acticity < 0)
        {
            perror("select failed");
            exit(1);
        }
        if(FD_ISSET(sockfd,&readfds)){
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd < 0)
            {
                perror("accept failed");
                exit(1);
            }
            std::cout << "New client connected,socket fd is " << client_fd << std::endl;
        }
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets[i] == 0)
            {
                client_sockets[i] = client_fd;
                FD_SET(client_fd, &readfds);
                if (client_fd > max_fd)
                {
                    max_fd = client_fd;
                }
                break;
            }
        }
        for (int i = 0; i < MAX_CLIENTS;i++){
            client_fd = client_sockets[i];
            if (client_fd <= 0){
                continue;
            }
            if (FD_ISSET(client_fd, &temp_readfds))
            {
                char buffer[MAX_CLIENTS];
                ssize_t read_bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);
                if (read_bytes <= 0)
                {
                    std::cout << "Client FD " << client_fd << " disconnected." << std::endl;
                    close(client_fd);
                    FD_CLR(client_fd, &readfds);
                    client_sockets[i] = 0;
                    max_fd = sockfd;
                    for (int j = 0; j < MAX_CLIENTS; ++j)
                    {
                        if (client_sockets[j] > max_fd)
                        {
                            max_fd = client_sockets[j];
                        }
                    }
                }
                else
                {
                    buffer[read_bytes] = '\0';
                    std::cout << "recv" << read_bytes << std::endl;
                    ssize_t send_bytes = send(client_fd, buffer, strlen(buffer), 0);
                    if (send_bytes < 0)
                    {
                        perror("send error");
                        exit(1);
                    }
                }
            }
        }
    }
    for (int client_fd : client_sockets)
    {
        if (client_fd > 0)
            close(client_fd);
    }
    close(sockfd);
    return 0;
}