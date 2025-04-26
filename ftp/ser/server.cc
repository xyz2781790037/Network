#include "net.hpp"
#include <map>
const int dataPORT = 2121;
const int cmdPORT = 1026;
const int MAX_EVENTS = 100;
const std::string UPLOAD_DIR = "./uploads";
std::map<int, std::string> clients;
std::map<int, int> data_map;
int main()
{
    pathtask pt;
    secv sc;
    cmd handler;
    std::atomic<bool> runflag = false;
    pt.createDir(UPLOAD_DIR);
    Net net1, net2;
    Epoll epol;
    int server_fd = net1.socket1(AF_INET, SOCK_STREAM, 0);
    int data_fd = net2.socket1(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr, data_addr;
    net1.binlis(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr), server_addr, cmdPORT);
    net2.binlis(data_fd, (struct sockaddr *)&data_addr, sizeof(data_addr), data_addr, dataPORT);
    epol.set_nonblocking(server_fd);
    epol.set_nonblocking(data_fd);
    int epoll_fd = epol.epoll_create2(server_fd, data_fd);
    epoll_event event1, event2;
    epol.epoll_ctl1(epoll_fd, server_fd, event1);
    epol.epoll_ctl1(epoll_fd, data_fd, event2);
    std::vector<epoll_event> events(MAX_EVENTS);
    std::cout << "Server listening on port " << cmdPORT << "&" << dataPORT << std::endl;
    pool po(32);
    while (true)
    {
        runflag = true;
        int n = epol.epoll_wait1(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++)
        {
            int fd = events[i].data.fd;
            if (fd == server_fd)
            {
                while (true)
                {
                    sockaddr_in client_addr;
                    int client_fd = net1.accept2(client_addr, epol, epoll_fd);
                    if (client_fd == -1)
                    {
                        break;
                    }
                    clients[client_fd] = inet_ntoa(client_addr.sin_addr);
                }
            }
            else if (fd == data_fd)
            {
                while (true)
                {
                    sockaddr_in cdata_addr;
                    int cdata_fd = net2.accept2(cdata_addr, epol, epoll_fd);
                    if (cdata_fd == -1)
                    {
                        break;
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
            }
            else if (events[i].events & EPOLLIN)
            {
                std::string buf;
                int ret = sc.recv1(fd, buf, 1024, 0);

                if (ret > 0)
                {
                    std::cout << "命令：" << buf << std::endl;
                    if(data_map.find(fd) != data_map.end()){
                        int dfd = data_map[fd];
                        po.enqueue([&net1, buf, fd, dfd, &runflag]() mutable
                                   { net1.handclient(dfd, fd, buf, runflag); });
                        while(runflag){
                        }
                        // shutdown(dfd, SHUT_WR);
                        close(dfd);
                        epol.epoll_ctl3(epoll_fd, dfd);
                        data_map.erase(fd);
                        std::cout << "客户端断开（fd " << dfd << "）" << std::endl;
                    }
                    else{
                        sendResponse(425, "No data connection established.", fd);
                    }
                }
                else if (ret == -2)
                {
                    std::cout << "客户端断开（fd " << fd << "）" << std::endl;
                    close(fd);
                    epol.epoll_ctl3(epoll_fd, fd);
                    clients.erase(fd);
                    if (data_map.count(fd))
                    {
                        int dfd = data_map[fd];
                        close(dfd);
                        epol.epoll_ctl3(epoll_fd, dfd);
                        data_map.erase(fd);
                    }
                }
                else
                {
                    if (ret == 0)
                    {
                        break;
                    }
                    std::cout << "接收出错，关闭: " << fd << std::endl;
                    close(fd);
                    epol.epoll_ctl3(epoll_fd, fd);
                    clients.erase(fd);
                    
                }
            }
        }
    }
    return 0;
}