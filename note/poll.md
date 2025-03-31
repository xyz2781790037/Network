# poll
### 一、poll函数介绍

[理解IO复用的三种模式——select、poll、epoll](https://blog.csdn.net/qq_41822345/article/details/134862963)

poll 函数用于在指定的超时时间内监视一组文件描述符，并返回文件描述符上是否有指定的I/O事件发生。
```c
#include <poll.h>
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

- **`fds`**：是一个数组，每个元素是一个`pollfd`结构，描述一个文件描述符及其要监视的事件。
- **`nfds`**：要监视的文件描述符个数。
- **`timeout`**：等待的超时时间（以毫秒为单位）。-1表示无限等待，0表示立即返回（非阻塞模式）。

##### `pollfd` 结构体

```c
struct pollfd { int fd; // 要监视的文件描述符
short events; // 等待的事
short revents; // 实际发生的事件
};
```

- fd：要监视的文件描述符，例如套接字或管道。
- events：感兴趣的事件，可以是以下的值的组合：

```bush
POLLIN：有数据可读。
POLLOUT：可以写数据（不会阻塞）。
POLLERR：发生错误。
POLLHUP：挂起事件（对方关闭连接）。
POLLNVAL：非法的文件描述符。
```

revents：poll返回时，实际发生的事件。

##### 返回值

- 成功时，返回大于0的值，表示有多少文件描述符有事件发生。
- 如果超时且无事件发生，返回0。
- 失败时，返回-1，并设置`errno`。

#### 二、`poll` 的使用步骤

1. **创建并初始化`pollfd`数组**：为需要监控的文件描述符设置监视事件。
2. **调用`poll`函数**：传入`pollfd`数组、数组大小和超时时间。
3. **处理事件**：根据返回的`revents`判断哪个文件描述符有事件发生，并做出相应处理。

#### 三、`poll` 示例

下面是一个使用 `poll` 监视两个套接字的简单例子：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
#define PORT 8080
#define MAX_EVENTS 2
 
int main() {
    int listenfd, connfd;
    struct sockaddr_in serv_addr;
    struct pollfd fds[MAX_EVENTS];
    int nfds = 1;
 
    // 创建监听套接字
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
 
    // 绑定并监听端口
    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }
 
    if (listen(listenfd, 3) < 0) {
        perror("listen failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }
 
    // 初始化pollfd数组
    fds[0].fd = listenfd;
    fds[0].events = POLLIN;
 
    printf("Waiting for connections...\n");
 
    while (1) {
        int ret = poll(fds, nfds, -1);  // 无限等待事件
 
        if (ret < 0) {
            perror("poll failed");
            exit(EXIT_FAILURE);
        }

        // 检查监听套接字是否有新连接
        if (fds[0].revents & POLLIN) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
 
            if ((connfd = accept(listenfd, (struct sockaddr*)&client_addr, &addr_len)) < 0) {
                perror("accept failed");
                exit(EXIT_FAILURE);
            }
 
            printf("New connection accepted\n");
        }
    }
 
    close(listenfd);
    return 0;
}
```

这个例子中，程序首先创建了一个监听套接字，然后使用 `poll` 函数监视这个套接字的 `POLLIN` 事件（有新的连接到来）。当有新连接时，程序通过 `accept` 函数接收连接。
