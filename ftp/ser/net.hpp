#include <netinet/in.h>
#include <arpa/inet.h>
#include "cmd.hpp"
#include "epoll.hpp"
#include "threadpool.hpp"
const int BUFFER_MAXSIZE = 1024;
class Net
{
    int server_fd;

public:
    int socket1(int domain, int type, int protocol)
    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0)
        {
            perror("socket");
            exit(1);
        }
        return server_fd;
    }
    void binlis(int fd, const sockaddr *addr, socklen_t len, struct sockaddr_in server_addr, int PORT)
    {
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            perror("bind");
            close(server_fd);
            exit(1);
        }
        if (listen(server_fd, 100) < 0)
        {
            perror("listen");
            close(server_fd);
            exit(1);
        }
    }
    int accept1(int fd, struct sockaddr_in &client_addr)
    {
        socklen_t cli_sock_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &cli_sock_len);
        if (client_fd < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return -1;
            perror("accept");
            exit(1);
        }
        return client_fd;
    }
    void recmd(int client_fd, std::string &buffer)
    {
        secv person;
        person.recv1(client_fd, buffer, BUFFER_MAXSIZE, 0);
    }
    void handclient(int data_fd, int client_fd, std::string &buffer,std::atomic<bool> &flag)
    {
        cmd sendorder;
        std::string order = buffer;
        sendorder.handcmd(order, client_fd, data_fd,flag);
        flag = false;
    }
    int accept2(struct sockaddr_in &client_addr, Epoll &epol, int epoll_fd)
    {
        int client_fd = accept1(server_fd, client_addr);
        if (client_fd == -1)
        {
            return -1;
        }
        epol.set_nonblocking(client_fd);
        epoll_event client_event;
        epol.epoll_ctl1(epoll_fd, client_fd, client_event);
        std::cout << "New client connected: " << client_fd << std::endl;
        return client_fd;
    }
};