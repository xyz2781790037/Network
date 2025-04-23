#include <iostream>
#include <sys/socket.h>
#include <cstring>
class resv{
public:

    int send1(int fd,std::string buf,int n,int flag,const char* a){
        char buffer[n];
        strcpy(buffer, buf.c_str());
        int send_bytes = send(fd, buffer, n, flag);
        if(send_bytes <= 0){
            perror(a);
            exit(1);
        }
        std::cout << a << ": " << buffer << std::endl;
        return send_bytes;
    }
    int recv1(int fd,std::string buf,int n,int flag,const char* a){
        char buffer[n];
        strcpy(buffer, buf.c_str());
        int recv_bytes = recv(fd, buffer, n, flag);
        if(recv_bytes <= 0){
            std::cout << "perror ";
            perror(a);
            return 0;
        }
        std::cout << a << ": " << buffer << std::endl;
        return recv_bytes;
    }
};