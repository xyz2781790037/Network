# select

## 第一章：select基础

### 1.1 select概述

select是C语言中用于IO多路复用的一个系统调用，它允许程序监视多个文件描述符（通常是网络套接字）的状态，以便在某个文件描述符准备好进行IO操作时获得通知。select的这种能力使得单个线程能够管理多个并发连接，从而实现高效的并发处理。

### 1.2 select的使用场景

select通常用于以下场景：

- 当需要同时处理多个套接字时，例如在网络服务器中。
- 当需要同时监听标准输入和其他套接字时。
- 当需要等待多个套接字事件（如读就绪、写就绪、异常）时。

### 1.3 select的函数原型

```c
#include <sys/select.h>
 
int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
```

- nfds：监视的文件描述符集合中最大文件描述符加1。
- readfds：指向fd_set结构的指针，用于监视读就绪的文件描述符。
- writefds：指向fd_set结构的指针，用于监视写就绪的文件描述符。
- exceptfds：指向fd_set结构的指针，用于监视异常的文件描述符。
- timeout：指向timeval结构的指针，用于设置select的超时时间。

### 1.4 fd_set和timeval结构体

d_set是一个位掩码，用于表示文件描述符集合。在select中，它用来指定需要监视的文件描述符。timeval[结构体](https://so.csdn.net/so/search?q=结构体&spm=1001.2101.3001.7020)用于指定select函数的超时时间。

```c
struct timeval {
    long tv_sec;  // 秒
    long tv_usec; // 微秒
};//两个一起用
```

### 1.5 select的基本流程

使用select的基本流程如下：

1. 初始化fd_set集合，将需要监视的文件描述符加入到相应的集合中。
2. 调用select函数，等待文件描述符就绪或超时。
3. 检查select返回值，处理就绪的文件描述符。
4. 根据需要重复以上步骤。

### 1.6 select的局限性

- fd_set集合的大小受限于系统定义的FD_SETSIZE，通常为1024，这意味着select不能有效地处理超过1024个并发连接。
- 每次调用select都需要重新设置文件描述符集合，这在有大量文件描述符时效率较低。
- select返回后，需要遍历整个文件描述符集合来确定哪些文件描述符就绪，这在高并发场景下可能成为性能瓶颈。

## 第二章：select的高级使用

### 2.1 处理select的超时

在select中，超时参数`timeout`可以用来指定select函数等待的最大时间。如果在这个时间内没有任何文件描述符就绪，select将返回0。正确处理select的超时对于实现高效的网络程序至关重要。

#### 2.1.1 设置永久等待

如果要使select永久等待，直到至少一个文件描述符就绪，可以将`timeout`设置为NULL：

```c
struct timeval timeout;
timeout.tv_sec = 0;
timeout.tv_usec = 0;
 
int ret = select(nfds, &readfds, &writefds, &exceptfds, NULL);
```

#### 2.1.2 设置非阻塞等待

如果要使select在指定的时间后超时，可以设置`timeout`结构体的`tv_sec`和`tv_usec`字段：

```c
struct timeval timeout;
timeout.tv_sec = 5;  // 等待5秒
timeout.tv_usec = 0; // 微秒
 
int ret = select(nfds, &readfds, &writefds, &exceptfds, &timeout);
```

### 2.2 避免select的局限性

#### 2.2.1 使用poll代替select

poll是另一个IO多路复用函数，它没有select的文件描述符数量限制，并且提供了更高效的接口来处理大量的文件描述符。

#### 2.2.2 使用epoll

epoll是Linux特有的IO多路复用机制，它解决了select和poll的一些问题，如文件描述符数量限制和效率问题。epoll使用事件驱动的机制，可以高效地处理大量的文件描述符。

### 2.3 select的错误处理

select在调用过程中可能会遇到错误，常见的错误包括：

- EBADF：文件描述符集合中有无效的文件描述符。
- EINTR：select被信号中断。
- EINVAL：select的参数无效，如`nfds`小于0或`timeout`的值非法。

### 2.4 select在多线程环境中的应用

在多线程环境中，select可以与线程结合起来使用，以提高程序的性能和并发处理能力。例如，可以在一个线程中使用select来监听多个套接字，然后在其他线程中处理就绪的套接字。
## FD
在 select 服务器中，需要用到一组与 文件描述符 (FD, File Descriptor) 相关的函数，主要包括以下几个：

### 1. `FD_ZERO(fd_set *set)`

- 作用：清空 `fd_set` 集合，初始化为空。

```c
fd_set read_fds;
FD_ZERO(&read_fds);
```

### 2. `FD_SET(int fd, fd_set *set)`

- 作用：向 `fd_set` 集合中添加文件描述符 `fd`。

```c
FD_SET(sockfd, &read_fds);
```

### 3. `FD_CLR(int fd, fd_set *set)`

- 作用：从 `fd_set` 集合中移除文件描述符 `fd`。

```c
FD_CLR(sockfd, &read_fds);
```

### 4. `FD_ISSET(int fd, fd_set *set)`

- 作用：检查 `fd_set` 集合中是否包含 `fd`，即该描述符是否有事件发生。
- 返回值：如果 `fd` 在 `set` 中，则返回非零值，否则返回 0。

```c
if (FD_ISSET(sockfd, &read_fds)) {
    printf("Socket %d is ready for reading\n", sockfd);
}
```

