// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <poll.h>
// #include <unistd.h>
// #include <string.h>
// #include <vector>
// #include <map>
// #define PORT 8089
// #define MAX_EVENTS 3
// #define BUFFER_MAX 1024
// struct Client
// {
//     int fd;
//     std::string send_buffer; // 存储要发送的数据
// };
// int main()
// {
//     int server_fd, client_fd;
//     server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd < 0)
//     {
//         perror("socket");
//         exit(1);
//     }
//     struct sockaddr_in server_addr;
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(PORT);
//     if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
//     {
//         perror("bind");
//         close(server_fd);
//         exit(1);
//     }
//     if (listen(server_fd, 5) < 0)
//     {
//         perror("listen");
//         close(server_fd);
//         exit(1);
//     }
//     std::cout << "listening :" << PORT << std::endl;
//     std::vector<pollfd> fds;
//     std::map<int, Client> clients; // 存储客户端数据
//     fds.push_back({server_fd, POLLIN, 0});
//     while (1)
//     {
//         int ret = poll(fds.data(), fds.size(), -1);
//         if (ret < 0)
//         {
//             perror("poll");
//             exit(1);
//         }
//         for (size_t i = 0; i < fds.size(); i++)
//         {
//             if (fds[i].revents & POLLIN)
//             {
//                 if(fds[i].fd == server_fd){
//                     struct sockaddr_in client_addr;
//                     socklen_t addr_len = sizeof(client_addr);
//                     client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
//                     if (client_fd < 0)
//                     {
//                         perror("accept");
//                         continue;
//                     }
//                     std::cout << "New client connected: " << client_fd << std::endl;
//                     fds.push_back({client_fd, POLLIN, 0});
//                     clients[client_fd] = {client_fd, ""};
//                 }
//                 else {
//                     char buffer[BUFFER_MAX];
//                     int
//                 }
//             }
//         }
//     }
//     close(server_fd);
//     return 0;
// }
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <string>

#define PORT 8089
#define BUFFER_MAX 1024

struct Client
{
    int fd;
    std::string send_buffer;
};

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Listening on port: " << PORT << std::endl;

    std::vector<pollfd> fds;
    std::map<int, Client> clients;
    fds.push_back({server_fd, POLLIN, 0});

    while (true)
    {
        int ret = poll(fds.data(), fds.size(), -1);
        if (ret < 0)
        {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == server_fd)
                {
                    // 处理新的客户端连接
                    struct sockaddr_in client_addr{};
                    socklen_t addr_len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
                    if (client_fd < 0)
                    {
                        perror("accept");
                        continue;
                    }
                    std::cout << "New client connected: " << client_fd << std::endl;
                    fds.push_back({client_fd, POLLIN, 0});
                    clients[client_fd] = {client_fd, ""};
                }
                else
                {
                    // 处理客户端数据
                    char buffer[BUFFER_MAX];
                    int recv_bytes = recv(fds[i].fd, buffer, BUFFER_MAX - 1, 0);
                    if (recv_bytes <= 0)
                    {
                        std::cout << "Client disconnected: " << fds[i].fd << std::endl;
                        close(fds[i].fd);
                        clients.erase(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        i--;
                    }
                    else
                    {
                        buffer[recv_bytes] = '\0';
                        std::cout << "Received: " << buffer << std::endl;

                        // 把数据存入 send_buffer，并监听 POLLOUT
                        clients[fds[i].fd].send_buffer += buffer;
                        fds[i].events |= POLLOUT;
                    }
                }
            }

            if (fds[i].revents & POLLOUT)
            {
                Client &client = clients[fds[i].fd];
                if (!client.send_buffer.empty())
                {
                    int send_bytes = send(fds[i].fd, client.send_buffer.c_str(), client.send_buffer.size(), 0);
                    if (send_bytes > 0)
                    {
                        client.send_buffer.erase(0, send_bytes);
                        if (client.send_buffer.empty())
                        {
                            fds[i].events &= ~POLLOUT; // 发送完毕，取消 POLLOUT 监听
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
