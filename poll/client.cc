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
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
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
        close(sock);
        exit(1);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        close(sock);
        exit(1);
    }

    std::cout << "Connected to server at " << SERVER_IP << ":" << SERVER_PORT << std::endl;
    std::vector<pollfd> fds;
    char buffer[BUFFER_MAX];
    while (true)
    {
        std::cout << "Enter message: ";
        std::string message;
        std::getline(std::cin, message);
        if (message == "exit")
            break;
        send(sock, message.c_str(), message.size(), 0);

        int recv_bytes = recv(sock, buffer, BUFFER_MAX - 1, 0);
        if (recv_bytes <= 0)
        {
            std::cout << "Server disconnected!" << std::endl;
            break;
        }

        buffer[recv_bytes] = '\0';
        std::cout << "Server: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}
