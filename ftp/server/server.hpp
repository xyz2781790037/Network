#include <sys/epoll.h>
#include <map>
#include "cmd.hpp"
#include "threadpool.hpp"
const int MAX_EVENTS = 100;
const char *SERVER_IP = "127.0.0.1";
const std::string UPLOAD_DIR = "./uploads";
std::map<int, std::atomic<bool>> pasv_fd;
std::map<int, int> data_map;
class FTP
{
    Net net1;
    Path path;
    // void epoll1(int &server_fd,Net &net1);
    void set_notblocking(int &fd);

public:
    void run();
};
void FTP::set_notblocking(int &fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
void FTP::run()
{
    path.createDir(UPLOAD_DIR);
    int server_fd = net1.socket1(AF_INET, SOCK_STREAM, 0);
    int data_fd = net1.socket1(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr, data_addr;
    net1.binlis(server_fd, sizeof(server_addr), server_addr, cmdPORT);
    net1.binlis(data_fd, sizeof(data_addr), data_addr, dataPORT);
    set_notblocking(server_fd);
    set_notblocking(data_fd);
    std::atomic<bool> runflag = false;
    pasv_fd[server_fd] = true;
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1");
        close(server_fd);
        return;
    }
    epoll_event event;
    event.data.fd = server_fd;
    event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
    epoll_event event2;
    event2.data.fd = data_fd;
    event2.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, data_fd, &event2);
    std::vector<epoll_event> events(MAX_EVENTS);
    std::cout << "Server listening on port " << cmdPORT << "&" << dataPORT << std::endl;
    pool po(32);
    while (true)
    {
        runflag = true;
        std::cout << "cycle" << std::endl;
        int n = epoll_wait(epoll_fd, events.data(), 100, -1);
        std::cout << "n: " << n << std::endl;
        for (int i = 0; i < n; i++)
        {
            int fd = events[i].data.fd;
            if (fd == server_fd)
            {
                while (true)
                {
                    std::cout << "fd: " << fd << std::endl;
                    sockaddr_in client_addr;
                    int client_fd = net1.accept1(server_fd, client_addr);
                    if (client_fd == -1)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            break;
                        }
                        else
                        {
                            perror("accept");
                            continue;
                        }
                    }
                    set_notblocking(client_fd);
                    epoll_event client_event;
                    client_event.data.fd = client_fd;
                    client_event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
                    std::cout << "New client connected: " << client_fd << std::endl;
                    clients[client_fd] = inet_ntoa(client_addr.sin_addr);
                }
            }
            else if (fd == data_fd)
            {
                sockaddr_in cdata_addr;
                int cdata_fd = 0;
                if (pasv_fd[server_fd])
                {
                    cdata_fd = net1.accept1(data_fd, cdata_addr);
                    if (cdata_fd <= 0)
                    {
                        continue;
                    }
                    if (!clients.empty())
                    {
                        std::string ip = inet_ntoa(cdata_addr.sin_addr);
                        for (const auto &[cmd_fd, stored_ip] : clients)
                        {
                            if (stored_ip == ip)
                            {
                                data_map[cmd_fd] = cdata_fd;
                                std::cout << cdata_fd << std::endl;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    net1.connect1(data_fd, clients[fd].c_str(), data_addr, dataPORT);
                    cdata_fd = data_fd;
                }
                std::cout << "New data connected: " << cdata_fd << std::endl;
            }
            else
            {
                std::string buf;
                int ret = net1.recv1(fd, buf, 1024, 0);

                if (ret > 0)
                {
                    if (data_map.find(fd) != data_map.end())
                    {
                        int cdata_fd = data_map[fd];
                        std::cout << "命令：" << buf << std::endl;
                        std::atomic<bool> &pasv = pasv_fd[server_fd];
                        po.enqueue([buf, fd, cdata_fd, &runflag, &pasv]() mutable
                                   { 
                                Net net;
                                net.handle_client(cdata_fd, fd, buf, runflag,pasv); });
                        while (runflag)
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        }
                        std::cout << "客户端断开（fd " << cdata_fd << "）" << std::endl;
                        close(cdata_fd);
                    }
                    else{
                        net1.send_Response(425, "No data connection established.", fd);
                    }
                }
                else if (ret == 0)
                {
                    break;
                }
                else if (ret == -2)
                {
                    std::cout << "客户端断开（fd " << fd << "）" << std::endl;
                    close(fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                    clients.erase(fd);
                    if (data_map.count(fd))
                    {
                        int dfd = data_map[fd];
                        close(dfd);
                        data_map.erase(fd);
                    }
                }
                else
                {
                    std::cout << "接收出错，关闭: " << fd << std::endl;
                    close(fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                    clients.erase(fd);
                    if (data_map.count(fd))
                    {
                        int dfd = data_map[fd];
                        close(dfd);
                        data_map.erase(fd);
                    }
                }
            }
            std::cout << "111" << std::endl;
        }
    }
};