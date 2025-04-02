# epoll
### 相关函数介绍
#### epoll_create函数
epoll_create函数用于创建epoll文件描述符，该文件描述符用于后续的epoll操作，参数size目前还没有实际用处，我们只要填一个大于0的数就行。
```c
#include <sys/epoll.h>
 
int epoll_create(int size);
 
// 参数：
// size:目前内核还没有实际使用，只要大于0就行
 
// 返回值：
// 返回epoll文件描述符
```
#### epoll_ctl函数
epoll_ctl函数用于增加，删除，修改epoll事件，epoll事件会存储于内核epoll结构体红黑树中。
```c
#include <sys/epoll.h>
 
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
 
参数：
epfd：epoll文件描述符
op：操作码
EPOLL_CTL_ADD:插入事件
EPOLL_CTL_DEL:删除事件
EPOLL_CTL_MOD:修改事件
fd：事件绑定的套接字文件描述符
events：事件结构体
 
返回值：
成功：返回0
失败：返回-1
```
#### struct epoll_event结构体
```c
#include <sys/epoll.h>
 
struct epoll_event{
  uint32_t events; //epoll事件，参考事件列表 
  epoll_data_t data;
};
typedef union epoll_data {  
    void *ptr;
    int fd;  //套接字文件描述符
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;
```
#### epoll事件列表：
```c
头文件：<sys/epoll.h>
 
enum EPOLL_EVENTS
{
    EPOLLIN = 0x001, //读事件
    EPOLLPRI = 0x002,
    EPOLLOUT = 0x004, //写事件
    EPOLLRDNORM = 0x040,
    EPOLLRDBAND = 0x080,
    EPOLLWRNORM = 0x100,
    EPOLLWRBAND = 0x200,
    EPOLLMSG = 0x400,
    EPOLLERR = 0x008, //出错事件
    EPOLLHUP = 0x010, //出错事件
    EPOLLRDHUP = 0x2000,
    EPOLLEXCLUSIVE = 1u << 28,
    EPOLLWAKEUP = 1u << 29,
    EPOLLONESHOT = 1u << 30,
    EPOLLET = 1u << 31 //边缘触发
  };
```
epoll_wait函数
```c
#include <sys/epoll.h>
 
int epoll_wait(int epfd, struct epoll_event *events,              
int maxevents, int timeout);
 
// 参数：
// epfd：epoll文件描述符
// events：epoll事件数组
// maxevents：epoll事件数组长度
// timeout：超时时间
// 小于0：一直等待
// 等于0：立即返回
// 大于0：等待超时时间返回，单位毫秒
// 返回值：
// 小于0：出错
// 等于0：超时
// 大于0：返回就绪事件个数
```
### LT模式和ET模式

##### LT（Level Triggered，水平触发）

**特点**：

- 只要文件描述符（fd）**仍然可读或可写**，`epoll_wait` **会持续返回** 该事件，直到数据被完全处理。
- **默认模式**，更容易使用，适用于大多数场景。

**适用场景**：

- 适用于**阻塞或非阻塞**的 I/O 。
- 适合**普通网络通信**，例如基于 `poll` 或 `select` 的程序转换为 `epoll`。

```c
while (true) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    for (int i = 0; i < nfds; i++) {
        if (events[i].events & EPOLLIN) {  // 只要有数据就会一直触发
            read(fd, buffer, sizeof(buffer));
        }
    }
}
```

##### ET（Edge Triggered，边缘触发）

**特点**：

- **只有在状态变化时触发事件**（例如**从不可读变为可读**）。
- 如果应用程序**没有一次性读完数据，后续不会再收到事件通知**，除非新的数据到来。
- **必须使用非阻塞 I/O** (`fcntl(fd, F_SETFL, O_NONBLOCK);`)，否则可能导致程序**卡住**。

**适用场景**：

- 适用于**高性能服务器**，减少 `epoll_wait` 的调用次数，提高吞吐量。
- 需要**一次性读取所有数据**，否则可能错过事件。

```c
while (true) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    for (int i = 0; i < nfds; i++) {
        if (events[i].events & EPOLLIN) {
            while (true) { // 必须循环读取，防止数据未读完
                int n = read(fd, buffer, sizeof(buffer));
                if (n == -1 && errno == EAGAIN) break;  // 没数据可读时退出
            }
        }
    }
}
```
### 阻塞和非阻塞

讨论epoll阻塞和非阻塞得从两方面讨论：epoll阻塞和epoll监听套接字阻塞。

- epoll阻塞：epoll自身是阻塞的，我们可以通过epoll_wait超时参数设置epoll阻塞行为。
    
- epoll监听套接字阻塞：epoll监听套接字阻塞是指插入epoll监听事件的套接字设置为阻塞模式。

epoll监听套接字设置成阻塞还是非阻塞？

这个问题可以肯定的回答是非阻塞，因为epoll是为高并发设计的，任何的其他阻塞行为，都会影响epoll高效运行。
