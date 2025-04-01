#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <vector>
#include <map>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#define PORT 8089
#define BUFFER_MAXSIZE 1024
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
    struct sockaddr_in server_addr;
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
        for (int i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == server_fd)
                {
                    struct sockaddr_in client_addr;
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
                    char buffer[BUFFER_MAXSIZE];
                    int recv_bytes = recv(fds[i].fd, buffer, BUFFER_MAXSIZE - 1, 0);
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
                        clients[fds[i].fd].send_buffer = buffer;
                        fds[i].events |= POLLOUT;
                    }
                }
            }
            if(fds[i].revents & POLLOUT){
                Client &client = clients[fds[i].fd];
                if(!client.send_buffer.empty()){
                    int send_bytes = send(fds[i].fd, client.send_buffer.c_str(), client.send_buffer.size(), 0);
                    if(send_bytes > 0){
                        client.send_buffer.erase(0,send_bytes);
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