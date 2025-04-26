#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
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
    int recvfile(int &read_fd, int &write_fd, size_t n)
    {
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
            send1(read_fd, "send ok", 7, 0, "send1");
            return 0;
        }
        else
        {
            std::cout << "read fail" << std::endl;
            send1(read_fd, "send file", 9, 0, "send1");
            return -1;
        }
    }
};