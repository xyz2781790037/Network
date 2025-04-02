#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LISTEN_BACKLOG (5)
#define BUF_SIZE (1500)

#define REQUEST_STR "tcp pack"

void usage(void)
{
    printf("*********************************\n");
    printf("./client 对端ip 对端端口\n");
    printf("*********************************\n");
}

int main(int argc, char *argv[])
{
    struct sockaddr_in client;
    struct sockaddr_in server;
    int sock_fd = 0;
    int ret = 0;
    socklen_t addrlen = 0;
    char send_buf[BUF_SIZE] = {0};
    char recv_buf[BUF_SIZE] = {0};

    if (argc != 3)
    {
        usage();
        return -1;
    }

    char *ip = argv[1];
    unsigned short port = atoi(argv[2]);
    printf("ip:port->%s:%u\n", argv[1], port);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
    {
        perror("socket error");
        return -1;
    }

    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    ret = connect(sock_fd, (struct sockaddr *)&server, sizeof(struct sockaddr));
    if (ret == -1)
    {
        close(sock_fd);
        perror("connect error");
        return -1;
    }

    char seq = 0x31;
    while (1)
    {
        memset(send_buf, seq, BUF_SIZE);
        send(sock_fd, send_buf, BUF_SIZE, 0);
        printf("send %s\n", send_buf);
        sleep(2);
        seq++;
    }

    close(sock_fd);

    return 0;
}