#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <iostream>
class Epoll
{
public:
    void set_nonblocking(int fd)
    {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
    int epoll_create2(int &fd)
    {
        int epoll_fd = epoll_create1(0);
        if (epoll_fd < 0)
        {
            perror("epoll_create1");
            close(fd);
            exit(1);
        }
        return epoll_fd;
    }
    void epoll_ctl1(int efd, int fd, epoll_event&event){
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
        epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event);
    }
    void epoll_ctl2(int efd, int fd, epoll_event &event)
    {
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        epoll_ctl(efd, EPOLL_CTL_MOD, fd, &event);
    }
    void epoll_ctl3(int efd, int fd)
    {
        epoll_ctl(efd, EPOLL_CTL_DEL, fd, nullptr);
    }
    int epoll_wait1(int efd, std::vector<epoll_event> &events, int maxevents, int timeout)
    {
        int n = epoll_wait(efd, events.data(), maxevents, -1);
        return n;
    } 
};