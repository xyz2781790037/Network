#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

void set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("socket");
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        close(sock_fd);
        return 1;
    }

    set_nonblocking(sock_fd);
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1");
        close(sock_fd);
        return 1;
    }

    epoll_event event;
    event.data.fd = sock_fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event);

    epoll_event stdin_event;
    stdin_event.data.fd = STDIN_FILENO;
    stdin_event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);

    std::cout << "Connected to chat server at " << SERVER_IP << ":" << SERVER_PORT << std::endl;

    std::vector<epoll_event> events(2);
    char buffer[BUFFER_SIZE];

    while (true)
    {
        int n = epoll_wait(epoll_fd, events.data(), 2, -1);
        for (int i = 0; i < n; i++)
        {
            if (events[i].data.fd == STDIN_FILENO)
            { // 处理用户输入
                std::string message;
                std::getline(std::cin, message);
                if (message == "exit")
                {
                    close(sock_fd);
                    return 0;
                }
                send(sock_fd, message.c_str(), message.size(), 0);
            }
            else if (events[i].data.fd == sock_fd)
            { // 处理服务器消息
                while (true)
                {
                    int bytes_read = read(sock_fd, buffer, BUFFER_SIZE);
                    if (bytes_read <= 0)
                        break;
                    buffer[bytes_read] = '\0';
                    std::cout << "Server: " << buffer << std::endl;
                }
            }
        }
    }

    close(sock_fd);
    return 0;
}
