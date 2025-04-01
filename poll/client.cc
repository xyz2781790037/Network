#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <poll.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8089
#define BUFFER_MAX 1024

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(sockfd);
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    std::cout << "Connected to server at " << SERVER_IP << ":" << SERVER_PORT << std::endl;
    std::vector<pollfd> fds;
    fds.push_back({0, POLLIN, 0});
    fds.push_back({sockfd, POLLIN, 0});
    char buffer[BUFFER_MAX];
    while (true)
    {
        std::cout << "Enter message: ";
        std::cout.flush(); // 确保立即输出
        int ret = poll(fds.data(), fds.size(), -1);
        if (ret < 0)
        {
            perror("poll");
            break;
        }
        if (fds[0].revents & POLLIN)
        {
            std::string message;
            std::getline(std::cin, message);
            if (message == "exit")
                break;
            send(sockfd, message.c_str(), message.size(), 0);
        }
        if (fds[1].revents & POLLIN)
        {
            int recv_bytes = recv(sockfd, buffer, BUFFER_MAX - 1, 0);
            if (recv_bytes <= 0)
            {
                std::cout << "Server disconnected!" << std::endl;
                break;
            }
            buffer[recv_bytes] = '\0';
            // std::cout << "Server: " << buffer << std::endl;
            std::cout << "Message from other client: " << buffer << std::endl;
        }
    }
    close(sockfd);
    return 0;
}
