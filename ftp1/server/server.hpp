#include <sys/epoll.h>
#include <map>
#include "cmd.hpp"
#include "threadpool.hpp"
const int dataPORT = 2121;
const int cmdPORT = 1026;
const int MAX_EVENTS = 100;
const std::string UPLOAD_DIR = "./uploads";
std::map<int, std::string> clients;
class FTP{
    Net net1;
    Path path;
    void epoll1(int &sock_fd,int &data_fd,Net &net1);
    void set_notblocking(int&fd);

public:
    void run();
};
void FTP::set_notblocking(int&fd){
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
void FTP::run(){
    std::atomic<bool> pasv = false;
    path.createDir(UPLOAD_DIR);
    int server_fd = net1.socket1(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    net1.binlis(server_fd, sizeof(server_addr), server_addr, cmdPORT);
    set_notblocking(server_fd);
    int data_fd = 0;
    if (pasv)
    {
        data_fd = net1.socket1(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in data_addr;
        net1.binlis(data_fd,sizeof(data_addr), data_addr, dataPORT);
    }
    else{
        
    }
    epoll1(server_fd,data_fd,net1);
}
void FTP::epoll1(int &sock_fd,int &data_fd,Net &net1){
    std::atomic<bool> runflag = false;
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1");
        close(sock_fd);
        return;
    }
    epoll_event event;
    event.data.fd = sock_fd;
    event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event);
    std::vector<epoll_event> events(MAX_EVENTS);
    std::cout << "Server listening on port " << cmdPORT << "&" << dataPORT << std::endl;
    pool po(32);
    while(true){
        runflag = true;
        int n = epoll_wait(epoll_fd, events.data(), 100, -1);
        for (int i = 0;i < n;i++){
            int fd = events[i].data.fd;
            if (fd == sock_fd)
            {
                while (true)
                {
                    sockaddr_in client_addr;
                    int client_fd = net1.accept1(sock_fd, client_addr);
                    if (client_fd == -1)
                    {
                        break;
                    }
                    set_notblocking(client_fd);
                    epoll_event client_event;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD,client_fd, &client_event);
                    std::cout << "New client connected: " << client_fd << std::endl;
                    clients[client_fd] = inet_ntoa(client_addr.sin_addr);
                }
            }
            else{
                sockaddr_in cdata_addr;
                int cdata_fd = net1.accept1(data_fd, cdata_addr);
                std::cout << "New client connected: " << cdata_fd << std::endl;
                std::string buf;
                int ret = net1.recv1(fd, buf, 1024, 0);
                if(ret > 0){
                    std::cout << "命令：" << buf << std::endl;
                    po.enqueue([&net1, buf, fd, cdata_fd, &runflag]() mutable
                               { net1.handle_client(cdata_fd, fd, buf, runflag); });
                    while(runflag){}
                    std::cout << "客户端断开（fd " << cdata_fd << "）" << std::endl;
                    close(cdata_fd);
                }
                else if(ret == 0){
                    break;
                }
                else if(ret == -2){
                    std::cout << "客户端断开（fd " << fd << "）" << std::endl;
                    close(fd);
                    clients.erase(fd);
                }
                else{
                    std::cout << "接收出错，关闭: " << fd << std::endl;
                    close(fd);
                    clients.erase(fd);
                    close(cdata_fd);
                }
            }
        }
    }
}