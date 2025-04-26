#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
void sendResponse(int code, const std::string &message, int client_fd)
{
    std::string response = std::to_string(code) + " " + message + "\r\n";
    int a = send(client_fd, response.c_str(), response.size(), 0);
    if( a <= 0){
        perror("send _res");
        exit(1);
    }
    std::cout << "send :" << response << std::endl;
}
class secv
{
public:
    int recv1(int fd, std::string &buff, size_t n, int flags)
    {
        char buf[1024];
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
            // 客户端断开连接
            return -2;
        }

        buf[recv_cmd] = '\0';
        buff = std::string(buf);
        return recv_cmd;
    }
    int send1(int &read_fd, int &write_fd, size_t n)
    {
        // char buffers[1024];
        int read_bytes = 0;
        int index_read = 5;
    READ:
        char buffers[1024];
        std::cout << "reading " << std::endl;
        while ((read_bytes = read(read_fd, buffers, n)) > 0)
        {
            if (write(write_fd, buffers, read_bytes) != read_bytes)
            {
                perror("write");
                close(write_fd);
                sendResponse(550, "Write error during file transfer.", read_fd);
                return -1;
            }
        }
        if (read_bytes == 0)
        {
            std::cout << "read end" << std::endl;
            sendResponse(226, "Transfer complete.", read_fd);
            return 0;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                if(index_read > 0){
                    sleep(1);
                    index_read--;
                    goto READ;
                }
                else{
                    return 0;
                }
            }
            std::cout << "read fail" << std::endl;
            sendResponse(451, "Read error during file transfer.", read_fd);
            return -1;
        }
    }
};