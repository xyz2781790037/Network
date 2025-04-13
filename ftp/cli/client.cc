#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include "threadpool.hpp"
#include "handstr.cc"
const int PORT = 2121;
const int BUFFER_MAXSIZE = 1024;
const std::string UPLOAD_DIR = "./uploads";
const char* SERVER_IP = "127.0.0.1";
const std::string COLOUR1 = "\033[1;36m";
const std::string COLOUR2 = "\033[0m";
void handinput(int fd, std::string input)
{
    handstr hs;
    hs.inputseg(input,fd);
    // char buffer[BUFFER_MAXSIZE];
    // ssize_t recv_bytes = recv(fd, buffer, BUFFER_MAXSIZE, 0);
    // if (recv_bytes < 0)
    // {
    //     perror("recv");
    //     return;
    // }
}
int main()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0){
        perror("socket");
        exit(1);
    }
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &client_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(client_fd);
        exit(1);
    }
    if(connect(client_fd,(struct sockaddr *)&client_addr,sizeof(client_addr)) < 0){
        perror("connect");
        close(client_fd);
        exit(1);
    }
    std::cout << "已连接到服务器" << SERVER_IP << " " << PORT << std::endl;
    pool po;
    std::string input;
    while(true){
        std::cout << COLOUR1 << "ftp> " << COLOUR2;
        getline(std::cin, input);
        if(input == "exit"){
            break;
        }
        po.enqueue(handinput, client_fd, input);
    }
    close(client_fd);
    return 0;
}