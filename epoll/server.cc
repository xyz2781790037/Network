#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <netinet/in.h>
#include <map>
#define PORT 12345
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024
std::map<int, std::string> clients;
void set_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
int main(){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("socket");
        exit(1);
    }
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        close(server_fd);
        exit(1);
    }
    set_blocking(server_fd);
    int epoll_fd = epoll_create1(0);
    if(epoll_fd < 0){
        perror("epoll_create1");
        close(server_fd);
        exit(1);
    }
    epoll_event event;
    event.data.fd = server_fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
    std::vector<epoll_event> events(MAX_EVENTS);
    std::cout << "Server listening on port " << PORT << std::endl;
    while(true){
        // 在 epoll_wait() 的第四个参数 timeout 设为 - 1 的作用是让 epoll_wait() 进入阻塞模式，直到至少有一个事件发生才返回。
        int n = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, -1);
        for (int i = 0; i < n;i++){
            int fd = events[i].data.fd;
            if(fd == server_fd){
                // 确保 accept() 能够一次性接收所有待处理的连接 
                while (true)
                {
                    sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_len);
                    if(client_fd < 0){
                        // 在非阻塞模式下，如果当前没有可接受的新连接，accept 会返回 - 1 并将 errno 设置为 EAGAIN 或 EWOULDBLOCK。此时代码直接 break，表示没有更多待处理的连接。
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        perror("accept");
                        break;
                    }
                    set_blocking(client_fd);
                    // 在某些情况下，你可能希望将 accept() 和其他操作放在一个事件循环中（例如使用 epoll），这时你需要让 accept() 立即返回，而不是一直等待。
                    epoll_event client_event;
                    client_event.data.fd = client_fd;
                    client_event.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
                    clients[client_fd] = "";
                    std::cout << "New client connected: " << client_fd << std::endl;
                }
            }
            else if(events[i].events & EPOLLIN){
                char buffer[BUFFER_SIZE];
                while(true){
                    int bytes_read = read(fd, buffer, BUFFER_SIZE);
                    if(bytes_read < 0){
                        if(bytes_read == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)){
                            std::cout << "Client disconnected :" << fd << std::endl;
                            close(fd);
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                            clients.erase(fd);
                        }
                        break;
                    }
                    buffer[bytes_read] = '\0';
                    std::cout << "Received from " << fd << ": " << buffer << std::endl;
                    for (const auto &[client_fd, _] : clients)
                    {
                        if (client_fd != fd)
                        {
                            send(client_fd, buffer, bytes_read, 0);
                        }
                    }
                }
            }
        }
    }
    close(server_fd);
    return 0;
}