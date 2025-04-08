#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include "ispath.cc"
#include "threadpool.hpp"
#define PORT 2100
#define BUFFER_MAXSIZE 1024
const std::string UPLOAD_DIR = "./uploads";
// void set_blocking(int fd)
// {
//     int flags = fcntl(fd, F_GETFL, 0);
//     fcntl(fd, F_SETFL, flags | O_NONBLOCK);
// }
void handclient(int client_fd,pathtask pt){
    char buffer[BUFFER_MAXSIZE];
    ssize_t recv_bytes = read(client_fd, buffer, BUFFER_MAXSIZE);
    if(recv_bytes < 0){
        close(client_fd);
        return;
    }
    std::string commend(buffer);
    commend = pt.segstrspace(commend);
    pt.handlefilename(commend);
}
int main(){
    pathtask pt;
    pt.createDir(UPLOAD_DIR);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("socket");
        exit(1);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
        perror("bind");
        close(server_fd);
        exit(1);
    }
    if(listen(server_fd,100) < 0){
        perror("listen");
        close(server_fd);
        exit(1);
    }
    std::cout << "listen : " << PORT << std::endl;
    while(true){
        struct sockaddr_in client_addr;
        socklen_t sock_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sock_len);
        if(client_fd < 0){
            perror("accept");
            continue;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Connecting from :" << client_ip << std::endl;
        pool po;
        // po.enqueue()
    }
    return 0;
}