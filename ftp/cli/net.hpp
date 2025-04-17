#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "handstr.hpp"
class Net{
    int client_fd;
public:
    int socket1(int domain, int type, int protocol){
        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0)
        {
            perror("socket");
            exit(1);
        }
        return client_fd;
    }
    void connect1(int af, const char *SERVER_IP, struct sockaddr_in use_addr,int PORT)
    {
        use_addr.sin_family = AF_INET;
        use_addr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, SERVER_IP, &use_addr.sin_addr) <= 0)
        {
            perror("inet_pton");
            close(client_fd);
            exit(1);
        }
        if (connect(client_fd, (struct sockaddr *)&use_addr, sizeof(use_addr)) < 0)
        {
            perror("connect");
            close(client_fd);
            exit(1);
        }
    }
    void handinput(int client_fd, int data_fd,std::string input)
    {
        handstr hs;
        hs.inputseg(input, client_fd,data_fd);
    }
};