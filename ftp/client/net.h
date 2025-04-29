#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <atomic>
#include <map>
class Net{
public:
    int socket1(int domain, int type, int protocol);
    void connect1(int fd, const char *SERVER_IP, struct sockaddr_in &use_addr, int PORT);
    void binlis(int &sock_fd, socklen_t len, struct sockaddr_in &server_addr, int PORT);
    int accept1(int &sock_fd, struct sockaddr_in &client_addr);
    int send1(int fd, std::string &buf, int n, int flag, const char *a);
    int recv1(int fd, std::string &buf, int n, int flag, const char *a);
    int recvfile(int &read_fd, int &write_fd, size_t n);
};
int Net::socket1(int domain, int type, int protocol){
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("socket");
        return -1;
    }
    return client_fd;
} 
void Net::connect1(int client_fd, const char *SERVER_IP, struct sockaddr_in &use_addr, int PORT)
{
    use_addr.sin_family = AF_INET;
    use_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &use_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(client_fd);
        return;
    }
    if (connect(client_fd, (struct sockaddr *)&use_addr, sizeof(use_addr)) < 0)
    {
        perror("connect");
        close(client_fd);
        return;
    }
}
void Net::binlis(int &sock_fd, socklen_t len, struct sockaddr_in &server_addr, int PORT)
{
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(sock_fd);
        exit(1);
    }
    if (listen(sock_fd, 100) < 0)
    {
        perror("listen");
        close(sock_fd);
        exit(1);
    }
}
int Net::accept1(int &sock_fd, struct sockaddr_in &client_addr)
{
    socklen_t cli_sock_len = sizeof(client_addr);
    int client_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &cli_sock_len);
    if (client_fd < 0)
    {
        perror("accept");
        return -1;
    }
    return client_fd;
}
int Net::send1(int fd, std::string &buf, int n, int flag, const char *a){
    char buffer[n];
    strcpy(buffer, buf.c_str());
    int send_bytes = send(fd, buffer, n, flag);
    if (send_bytes <= 0)
    {
        perror("send1");
        return -1;
    }
    std::cout << a << ": " << buffer << std::endl;
    return send_bytes;
}
int Net::recv1(int fd, std::string &buf, int n, int flag, const char *a){
    char buffer[n + 1];
    int recv_bytes = recv(fd, buffer, n, flag);
    if (recv_bytes <= 0)
    {
        std::cout << "perror ";
        perror("recv1");
        return 0;
    }
    buffer[recv_bytes] = '\0';
    buf = buffer;
    std::cout << a << ": " << buffer << std::endl;
    return recv_bytes;
}
int Net::recvfile(int &read_fd, int &write_fd, size_t n){
    char buffer[1024];
    int read_bytes = 0;
    std::cout << "reading " << std::endl;
    while ((read_bytes = read(read_fd, buffer, n)) > 0)
    {
        if (write(write_fd, buffer, read_bytes) != read_bytes)
        {
            perror("write");
            close(write_fd);
            return -1;
        }
    }
    if (read_bytes == 0)
    {
        std::cout << "read end" << std::endl;
        std::string sent = "send ok";
        close(write_fd);
        send1(read_fd, sent, 7, 0, "send1");
        return 0;
    }
    else
    {
        std::cout << "read fail" << std::endl;
        std::string sent = "send file";
        close(write_fd);
        send1(read_fd, sent, 9, 0, "send1");
        return -1;
    }
}