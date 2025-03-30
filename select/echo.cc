#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#define MAX_CLIENTS 10
#define PORT 12345
#define BUFFER_SIZE 1024
int main()
{
    int server_fd, client_fd, max_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    fd_set readfds, temp_readfds;
    char buffer[BUFFER_SIZE];
    std::vector<int> client_sockets(10, 0);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        exit(1);
    }
    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("listen failed");
        exit(1);
    }
    std::cout << "正在监听窗口：12345" << std::endl;
    // 初始化fd_set
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);
    max_fd = server_fd;
    while (1)
    {
        temp_readfds = readfds;
        // select等待就绪文件描述符
        int acticity = select(max_fd + 1, &temp_readfds, nullptr, nullptr, nullptr);
        if (acticity < 0)
        {
            perror("select failed");
            exit(1);
        }
        // 检查服务器套接字
        if (FD_ISSET(server_fd, &temp_readfds))
        {
            client_addr_len = sizeof(client_addr);
            client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_fd < 0)
            {
                perror("accept failed");
                exit(1);
            }
            std::cout << "New client connected,socket fd is " << client_fd << std::endl;
        }
        // 添加到客户端套接字数组
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
            if(client_fd <= 0){
                continue;
            }
            if(FD_ISSET(client_fd,&temp_readfds)){
                ssize_t read_bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);
                if(read_bytes <= 0){
                    std::cout << "Client FD " << client_fd << " disconnected." << std::endl;
                    close(client_fd);
                    FD_CLR(client_fd, &readfds);
                    client_sockets[i] = 0;
                    max_fd = server_fd;
                    for (int j = 0; j < MAX_CLIENTS; ++j)
                    {
                        if (client_sockets[j] > max_fd)
                        {
                            max_fd = client_sockets[j];
                        }
                    }
                }
                else {
                    buffer[read_bytes] = '\0';
                    std::cout << "recv" << read_bytes << std::endl;
                    ssize_t send_bytes = send(client_fd, buffer, strlen(buffer), 0);
                    if(send_bytes < 0){
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
    close(server_fd);
    return 0;
}