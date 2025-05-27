#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <atomic>
#include <map>
#include <unistd.h>
const int dataPORT = 2121;
const int cmdPORT = 1026;
std::map<int, std::string> clients;
class Net{
public:
    int socket1(int domain, int type, int protocol);
    void binlis(int &sock_fd, socklen_t len, struct sockaddr_in &server_addr, int PORT);
    int accept1(int &sock_fd, struct sockaddr_in &client_addr);
    void connect1(int &fd, const char *SERVER_IP, struct sockaddr_in &use_addr, int PORT);
    void send_Response(int code, const std::string &message, int client_fd);
    int recv1(int fd, std::string &buff, size_t n, int flags);
    int recvfile(int &read_fd, int &write_fd, size_t n);
    void handle_client(int &data_fd, int &sock_fd, std::string input, std::atomic<bool> &runflag, std::atomic<bool> &pasv);
};
int Net::socket1(int domain, int type, int protocol){
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("socket");
        exit(1);
    }
    return sock_fd;
}
void Net::send_Response(int code, const std::string &message, int client_fd){
    std::string response = std::to_string(code) + " " + message + "\r\n";
    int a = send(client_fd, response.c_str(), response.size(), 0);
    if (a <= 0)
    {
        perror("send _res");
        exit(1);
    }
    std::cout << "send :" << response << std::endl;
}
void Net::binlis(int &sock_fd, socklen_t len, struct sockaddr_in &server_addr, int PORT){
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
int Net::accept1(int &sock_fd, struct sockaddr_in &client_addr){
    socklen_t cli_sock_len = sizeof(client_addr);
    int client_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &cli_sock_len);
    if (client_fd < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return -1;
        perror("accept");
        return -2;
    }
    return client_fd;
}
void Net::connect1(int &fd,const char *SERVER_IP, struct sockaddr_in &use_addr, int PORT)
{
    use_addr.sin_family = AF_INET;
    use_addr.sin_port = htons(PORT);
    std::cout << SERVER_IP << std::endl;
    if (inet_pton(AF_INET, SERVER_IP, &use_addr.sin_addr) <= 0)
    {
        
        if (errno == EINPROGRESS)
        {
            std::cout << "inet" << std::endl;
        }
        perror("inet_pton");
        close(fd);
        return;
    }
    if (connect(fd, (struct sockaddr *)&use_addr, sizeof(use_addr)) < 0)
    {
        perror("connect");
        close(fd);
        return;
    }
}
int Net::recv1(int fd, std::string &buff, size_t n, int flags)
{
    char buf[1025];
    int recv_cmd = recv(fd, buf, n, flags);

    if (recv_cmd < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return 0;
        }
        perror("recvcmd");
        return -1; // 真正出错
    }
    else if (recv_cmd == 0)
    {
        return -2;
    }

    buf[recv_cmd] = '\0';
    buff = std::string(buf);
    return recv_cmd;
}
int Net::recvfile(int &read_fd, int &write_fd, size_t n) {
    int read_bytes = 0;
    char buffers[1024];
    std::cout << "reading " << std::endl;
    while ((read_bytes = read(read_fd, buffers, n)) > 0)
    {
        if (write(write_fd, buffers, read_bytes) != read_bytes)
        {
            perror("write");
            close(write_fd);
            send_Response(550, "Write error during file transfer.", read_fd);
            return -1;
        }
    }
    if (read_bytes == 0)
    {
        std::cout << "read end" << std::endl;
        close(write_fd);
        send_Response(226, "Transfer complete.", read_fd);
        return 0;
    }
    else
    {
        std::cout << "read fail" << std::endl;
        close(write_fd);
        send_Response(451, "Read error during file transfer.", read_fd);
        return -1;
    }
};